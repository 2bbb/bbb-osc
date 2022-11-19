#include <bbb/osc/asio_receiver.hpp>
#include <iostream>

int main() {
    bbb::osc::asio_receiver receiver;
    receiver.setup(26666);
    receiver.bind("/hoge", [](const bbb::osc::message &m){
        std::cout << m << std::endl;
    });

    while(true) {
        receiver.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}