//
// The code is only for cpp. In other words, it doesn't care about the JNI ...
// Some adapter codes for Java will be placed in src/main/cpp
//
#pragma once

#include <type_traits>

#if __has_include(<WinSock2.h>) // use winsock
#include <WinSock2.h>

struct sockaddr_in6;

#elif __has_include(<netinet/in.h>) // use netinet
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#endif

void init_network_api() noexcept(false);
void release_network_api() noexcept;

//  create 1 socket
//  on failure, returns invalid handle value
int64_t socket_create(const addrinfo& hint) noexcept;

//  dispose given socket
//  on failure, returns error code
uint32_t socket_close(int64_t sd) noexcept;

//  bind the socket to given address
//  on failure, returns error code
uint32_t socket_bind(int64_t sd, const sockaddr_in& local) noexcept;
uint32_t socket_bind(int64_t sd, const sockaddr_in6& local) noexcept;

//  start listen with the socket
//  on failure, returns error code
uint32_t socket_listen(int64_t sd) noexcept;

//  try connect to given endpoint
//  on failure, returns error code
uint32_t socket_connect(int64_t sd, const sockaddr_in& remote) noexcept;
uint32_t socket_connect(int64_t sd, const sockaddr_in6& remote) noexcept;

//  accept a connection request and return client socket
uint32_t socket_accept(int64_t ln) noexcept;

//  get the socket's address
//  on failure, returns error code
uint32_t socket_get_name(int64_t sd, sockaddr_in& local) noexcept;
uint32_t socket_get_name(int64_t sd, sockaddr_in6& local) noexcept;

//  get connected peer socket's address
//  on failure, returns error code
uint32_t socket_get_peer(int64_t sd, sockaddr_in& remote) noexcept;
uint32_t socket_get_peer(int64_t sd, sockaddr_in6& remote) noexcept;

//  change the socket's option
uint32_t socket_set_option(int64_t sd, int64_t level, //
                           int64_t option, int64_t value) noexcept;

//  make socket to operate in non-blocking mode
uint32_t socket_set_option_nonblock(int64_t sd) noexcept;

//  make socket to reuse address
uint32_t socket_set_option_reuse_address(int64_t sd) noexcept;

//  make tcp send without delay
uint32_t socket_set_option_nodelay(int64_t sd) noexcept;

//  set the socket's send/recv timeout
uint32_t socket_set_option_timout(int64_t sd, //
                                  uint32_t send_ms, uint32_t recv_ms) noexcept;

//  network related error. It's from `errno` or `WSAGetLastError`
uint32_t recent_net_error() noexcept;

//  test if the error code is because of non-blocking
bool is_in_progress(uint32_t ec) noexcept;
