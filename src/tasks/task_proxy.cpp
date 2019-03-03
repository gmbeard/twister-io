#include "twister/tasks/task_proxy.hpp"

using namespace twister::tasks;

bool TaskProxy::poll() {
    return inner_->call();
}
