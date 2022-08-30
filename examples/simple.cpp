#include <bbb/osc.hpp>

#include <thread>
#include <chrono>
#include <iostream>

int main() {
    auto &manager = bbb::osc::receiver::manager::shared();
    auto receiver = manager.get(26666);
    receiver->add_callback("/test", [] {
        std::cout << "received /test" << std::endl;
    });
    while(true) {
        manager.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}