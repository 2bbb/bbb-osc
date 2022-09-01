#include <bbb/osc.hpp>

#include <thread>
#include <chrono>
#include <iostream>

namespace test {
    struct hoge_t {
        int x;
        float y;
        double z;
    };

    std::size_t to_osc(bbb::osc::message &m, const hoge_t &v) {
        m.push_list(v.x, v.y, v.z);
        return 3;
    }

    std::size_t from_osc(const bbb::osc::message &m, hoge_t &v, std::size_t offset = 0) {
        v.x = m[offset].get<int>();
        v.y = m[offset + 1];
        v.z = m[offset + 2];
        return 3;
    }
};

int main() {
    auto &manager = bbb::osc::receiver::manager::shared();
    auto receiver = manager.get(26666);
    receiver->bind("/test", [](const bbb::osc::message &m) {
        std::cout << "received /test ";
        for(auto i = 0; i < m.size(); ++i) {
            std::cout << m[i] << " ";
        }
        std::cout << std::endl;

        int n;
        test::hoge_t v;
        m.to(n, v);
        std::cout << "n " << n << " v.x " << v.x << " v.y " << v.y << " v.z " << v.z << std::endl;
    });

    bbb::osc::sender sender;
    sender.setup("127.0.0.1", 26666);

    int i = 0;
    test::hoge_t v{};
    while(true) {
        manager.update();
        if(i % 100 == 0) {
            sender.send("/test", 2, v);
            // sender.send("/test", 1, 2, 3);
            i = 0;
        }
        v.x++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        i++;
    }

    return 0;
}
