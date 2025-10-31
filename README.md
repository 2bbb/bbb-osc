# bbb-osc

## How to setup

```cpp
git clone --recursive https://github.com/2bbb/bbb-osc.git
```

## Simple Usage
```cpp
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

```

## build example

```bash
cmake -S . -B build
cmake --build build
```

## Dependencies

### asio

* [chriskohlhoff/asio](https://github.com/chriskohlhoff/asio)
* [Boost Software License - Version 1.0](https://github.com/chriskohlhoff/asio/blob/master/LICENSE_1_0.txt)

### oscpp (Original)

* [kaoskorobase/oscpp](https://github.com/kaoskorobase/oscpp)
* [MIT License](https://github.com/kaoskorobase/oscpp/blob/master/LICENSE)

### bit_by_bit

* [2bbb/bit_by_bit](https://github.com/2bbb/bit_by_bit)
* [MIT License](https://github.com/2bbb/bit_by_bit/blob/master/LICENSE)

## License

MIT License
