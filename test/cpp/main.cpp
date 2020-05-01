#include <clocale>

extern void test_env(int, char*[]);

int main(int argc, char* argv[]) {
    if (argc < 2)
        setlocale(LC_ALL, ".65001");

    test_env(argc, argv);
    return 0;
}
