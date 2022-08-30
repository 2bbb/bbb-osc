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

    bbb::osc::sender sender;
    sender.setup("127.0.0.1", 26666);

    int i = 0;
    while(true) {
        manager.update();
        if(i % 100 == 0) {
            sender.send("/test");
            i = 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        i++;
    }

    return 0;
}
