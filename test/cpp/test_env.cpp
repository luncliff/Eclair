#include <boost/ut.hpp>

#include "adapter.h"

namespace testing = boost::ut;

void test_env(int, char*[]) {
    using namespace testing;

    "env"_test = []() { testing::expect(is_test_mode()); };
}
