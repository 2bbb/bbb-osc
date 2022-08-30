//
//  bbb/udp/receiver.hpp
//
//  Created by 2bit on 2022/08/22.
//

#ifndef bbb_udp_receiver_hpp
#define bbb_udp_receiver_hpp

#include <bbb/udp/socket.hpp>

#include <array>
#include <exception>
#include <thread>

namespace bbb {
    namespace udp {
        struct receiver : public socket {
            receiver() : socket() {}
            
            void close() {
                sock.close();
                if(thread.joinable()) thread.join();
            }
            
            virtual ~receiver() {
                if(sock.is_open()) close();
            }
            
            BBB_NO_DISCARD bool setup(std::uint16_t port) {
                try {
                    auto endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port);
                    sock = boost::asio::ip::udp::socket(io_service,
                                                        endpoint);
                    binded_port = port;
                } catch(std::exception &e) {
                    std::cerr << "error on bbb:udpreceiver::setup " << e.what() << std::endl;
                    return false;
                }
                
                thread = std::thread([=]() {
                    boost::asio::ip::udp::endpoint remote_endpoint;
                    std::array<char, bbb::udp::buf_size> recv_buf;
                    while(sock.is_open()) {
                        boost::system::error_code error;
                        std::size_t len = this->sock.receive_from(boost::asio::buffer(recv_buf),
                                                                  remote_endpoint,
                                                                  0,
                                                                  error);
                        if(error) {
                            std::cerr << "error on sock[" << binded_port << "].receive_from in bbb:udp::receiver::setup.thread: \" " << error.message() << " \"" << std::endl;
                        } else {
                            this->receive(error, recv_buf, len);
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                });
                
                return true;
            }
        private:
            std::thread thread;
        protected:
            virtual void receive(const boost::system::error_code &error_code,
                                 std::array<char, bbb::udp::buf_size> &buf,
                                 std::size_t len)
            {
                std::cout.write(buf.data(), len);
            }
            std::uint16_t binded_port;
        }; // struct udpreceiver
    }; // namespace udp
}; // namespace bbb

#endif /* bbb_udp_receiver_hpp */
