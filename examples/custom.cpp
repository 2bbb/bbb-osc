#include <bbb/osc/asio_receiver.hpp>
#include <bbb/osc/asio_sender.hpp>
#include <iostream>

int main() {
    bbb::osc::asio_receiver receiver;
    receiver.setup(26666);
    receiver.bind("/hoge", [](const bbb::osc::message &m){
        std::cout << m << std::endl;
        std::cout << std::flush;
    });

    bbb::osc::asio_sender sender;
    sender.setup("127.0.0.1", 26667);

    int i = 0;
    while(true) {
        receiver.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        i++;
        if(i % 10 == 0) {
            sender.send("/foo", i, 2, 3);
        }
    }
    return 0;
}