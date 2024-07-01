/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/udp/receiver.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

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
                if(sock.is_open()) sock.close();
                if(main_loop.joinable()) main_loop.join();
            }
            
            virtual ~receiver() {
                if(sock.is_open()) close();
                if(main_loop.joinable()) main_loop.join();
            }
            
            BBB_NO_DISCARD bool setup(std::uint16_t port,
                                      std::string local_ip = "0.0.0.0")
            {
                try {
                    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(local_ip),
                                                            port);
                    sock = boost::asio::ip::udp::socket(io_context,
                                                        endpoint);
                    sock.non_blocking(true);
                    binded_port = port;
                    binded_local_ip = local_ip;
                } catch(std::exception &e) {
                    std::cerr << "error on bbb:udpreceiver::setup " << e.what() << std::endl;
                    return false;
                }
                
                main_loop = std::thread([=]() {
                    boost::asio::ip::udp::endpoint remote_endpoint;
                    std::array<char, bbb::udp::buf_size> recv_buf;
                    while(sock.is_open()) {
                        boost::system::error_code error;
                        std::size_t len = this->sock.receive_from(boost::asio::buffer(recv_buf),
                                                                  remote_endpoint,
                                                                  0,
                                                                  error);
                        if(error) {
                            if(error.value() != EAGAIN) {
                                std::cerr << "error on sock[" << binded_local_ip << ":" << binded_port << "].receive_from in bbb:udp::receiver::main_loop: \" " << error.message() << " \"" << std::endl;
                            }
                        } else {
                            this->receive(error, recv_buf, len, remote_endpoint);
                        }
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                    }
                });
                
                return true;
            }
        private:
            std::thread main_loop;
        protected:
            virtual void receive(const boost::system::error_code &error_code,
                                 std::array<char, bbb::udp::buf_size> &buf,
                                 std::size_t len,
                                 boost::asio::ip::udp::endpoint remote_endpoint) = 0;
            std::uint16_t binded_port;
            std::string binded_local_ip;
        }; // struct udpreceiver
    }; // namespace udp
}; // namespace bbb

#endif /* bbb_udp_receiver_hpp */
