#include "twister/tasks/task_proxy.hpp"
#include "testy/testy.hpp"

McTest(should_construct_task_proxy) {
    bool called = false;
    twister::tasks::TaskProxy proxy {[&] {
        called = true;
        return called;
    }};

    proxy();
    Expect(called);
}
