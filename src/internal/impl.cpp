
#include <cerrno>
#include <chrono>
#include <system_error>

#if __has_include(<WinSock2.h>) // use winsock
#include <WS2tcpip.h>
#include <ws2def.h>

#elif __has_include(<netinet/in.h>) // use netinet
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#endif

#include "impl.h"

using namespace std;

int64_t socket_create(const addrinfo& hint) noexcept {
    return ::socket(hint.ai_family, //
                    hint.ai_socktype, hint.ai_protocol);
}

uint32_t socket_bind(int64_t sd, const sockaddr_in& local) noexcept {
    if (::bind(sd, (const sockaddr*)&local, sizeof(sockaddr_in)))
        return recent_net_error();
    return 0;
}
uint32_t socket_bind(int64_t sd, const sockaddr_in6& local) noexcept {
    // bind socket and address
    if (::bind(sd, (const sockaddr*)&local, sizeof(sockaddr_in6)))
        return recent_net_error();
    return 0;
}

uint32_t socket_listen(int64_t sd) noexcept {
    ::listen(sd, 7);
    return recent_net_error();
}

uint32_t socket_connect(int64_t sd, const sockaddr_in& remote) noexcept {
    auto* ptr = reinterpret_cast<const sockaddr*>(&remote);
    if (::connect(sd, ptr, sizeof(sockaddr_in)) < 0)
        return recent_net_error();
    return 0;
}

uint32_t socket_connect(int64_t sd, const sockaddr_in6& remote) noexcept {
    auto* ptr = reinterpret_cast<const sockaddr*>(&remote);
    if (::connect(sd, ptr, sizeof(sockaddr_in6)) < 0)
        return recent_net_error();
    return 0;
}

uint32_t socket_accept(int64_t ln) noexcept {
    if (::accept(ln, nullptr, nullptr))
        return recent_net_error();
    return 0;
}

uint32_t socket_get_name(int64_t sd, sockaddr_in& local) noexcept {
    socklen_t len = sizeof(sockaddr_in);
    if (::getsockname(sd, (sockaddr*)&local, &len))
        return recent_net_error();
    return 0;
}

uint32_t socket_get_name(int64_t sd, sockaddr_in6& local) noexcept {
    socklen_t len = sizeof(sockaddr_in6);
    if (::getsockname(sd, (sockaddr*)&local, &len))
        return recent_net_error();
    return 0;
}

uint32_t socket_get_peer(int64_t sd, sockaddr_in& local) noexcept {
    socklen_t len = sizeof(sockaddr_in);
    if (::getpeername(sd, (sockaddr*)&local, &len))
        return recent_net_error();
    return 0;
}

uint32_t socket_get_peer(int64_t sd, sockaddr_in6& local) noexcept {
    socklen_t len = sizeof(sockaddr_in6);
    if (::getpeername(sd, (sockaddr*)&local, &len))
        return recent_net_error();
    return 0;
}

uint32_t socket_set_option(int64_t sd, int64_t level, //
                           int64_t option, int64_t value) noexcept {
    if (auto ec = ::setsockopt(sd, level, option, (char*)&value, sizeof(value)))
        return recent_net_error();
    return 0;
}

uint32_t socket_set_option_reuse_address(int64_t sd) noexcept {
    return socket_set_option(sd, SOL_SOCKET, SO_REUSEADDR, true);
}

uint32_t socket_set_option_nodelay(int64_t sd) noexcept {
    return socket_set_option(sd, IPPROTO_TCP, TCP_NODELAY, true);
}

uint32_t socket_set_option_timout1(int64_t sd, uint32_t ms,
                                   int64_t option) noexcept {
    constexpr auto unit = 1000; // millisecond
    timeval timeout{};
    timeout.tv_sec = ms / unit;
    timeout.tv_usec = (ms % unit) * unit;
    if (::setsockopt(sd, SOL_SOCKET, option, //
                     (char*)&timeout, sizeof(timeval)) != 0) {
        return recent_net_error();
    }
    return 0;
}

uint32_t socket_set_option_timout(int64_t sd, //
                                  uint32_t send_ms, uint32_t recv_ms) noexcept {
    if (auto ec = socket_set_option_timout1(sd, send_ms, SO_SNDTIMEO))
        return ec;
    if (auto ec = socket_set_option_timout1(sd, recv_ms, SO_RCVTIMEO))
        return ec;
    return 0;
}

#if __has_include(<WinSock2.h>) // using winsock

WSADATA wsa_data{};

void init_network_api() noexcept(false) {
    if (wsa_data.wVersion) // already initialized
        return;

    // init version 2.2
    if (::WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
        auto errc = WSAGetLastError();
        throw system_error{errc, system_category(), "WSAStartup"};
    }
}

void release_network_api() noexcept {
    // not initialized or released
    if (wsa_data.wVersion == 0)
        return;
    ::WSACleanup();
    wsa_data.wVersion = 0;
}

uint32_t recent_net_error() noexcept {
    return static_cast<uint32_t>(WSAGetLastError());
}

bool is_in_progress(int ec) noexcept {
    return ec == WSAEWOULDBLOCK || ec == EWOULDBLOCK || ec == EINPROGRESS ||
           ec == ERROR_IO_PENDING;
}

uint32_t socket_close(int64_t sd) noexcept {
    return shutdown(sd, SD_BOTH), closesocket(sd);
}

uint32_t socket_set_option_nonblock(int64_t sd) noexcept {
    u_long mode = TRUE;
    return ioctlsocket(sd, FIONBIO, &mode);
}

#elif __has_include(<netinet/in.h>) // using netinet

void init_network_api() noexcept(false) {
    // do nothing for posix system. network operation already available
}
void release_network_api() noexcept {
}

uint32_t recent_net_error() noexcept {
    return static_cast<uint32_t>(errno);
}

bool is_in_progress(int ec) noexcept {
    return ec == EINPROGRESS;
}

uint32_t socket_close(int64_t sd) noexcept {
    return shutdown(sd, SHUT_RDWR), close(sd), errno;
}

uint32_t socket_set_option_nonblock(int64_t sd) noexcept {
    // make non-block/async
    if (fcntl(sd, F_SETFL, O_NONBLOCK) != 0)
        return errno;
    return 0;
}

#endif
