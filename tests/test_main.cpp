#include "testy/testy.hpp"

McTest(fail_on_purpose) {
    Expect(false);
}

auto main() -> int {
    return testy::run_all_tests();
}
