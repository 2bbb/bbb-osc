#include <bbb/osc.hpp>

#include <thread>
#include <chrono>
#include <iostream>

namespace test {
    struct simple_struct {
        int x;
        float y;
        double z;
    };

    std::size_t to_osc(bbb::osc::message &m, const simple_struct &v) {
        m.push_list(v.x, v.y, v.z);
        return 3;
    }

    std::size_t from_osc(const bbb::osc::message &m, simple_struct &v, std::size_t offset = 0) {
        v.x = m[offset];
        v.y = m[offset + 1];
        v.z = m[offset + 2];
        return 3;
    }

    struct has_convert {
        int x;
        float y;
        double z;

        std::size_t to_osc(bbb::osc::message &m) const {
            m.push_list(x, y, z);
            return 3;
        }
        std::size_t from_osc(const bbb::osc::message &m, std::size_t offset = 0) {
            x = m[offset];
            y = m[offset + 1];
            z = m[offset + 2];
            return 3;
        }
    };
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
        test::simple_struct v;
        m.to(n, v);
        n = m[0];
        v = m.get<test::simple_struct>(1);
        std::cout << "n " << n << " v.x " << v.x << " v.y " << v.y << " v.z " << v.z << std::endl;

        // test::has_convert w;
        // w = m.get<test::has_convert>(1);
        // std::cout << "n " << n << " v.x " << v.x << " v.y " << v.y << " v.z " << v.z << std::endl;
    });

    bbb::osc::sender sender;
    sender.setup("localhost", 26666);

    int i = 0;
    test::simple_struct v{};
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
