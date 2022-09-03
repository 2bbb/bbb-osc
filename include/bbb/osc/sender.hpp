/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/sender.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_sender_hpp
#define bbb_osc_sender_hpp

#include <bbb/osc/message.hpp>
#include <bbb/osc/common.hpp>

#include <bbb/udp/sender.hpp>

namespace bbb {
    namespace osc {
        class sender : public bbb::udp::sender {
        public:
            using bbb::udp::sender::setup;
            
            template <typename ... ArgTypes>
            void send(const std::string &address,
                      ArgTypes && ... args)
            {
                send(bbb::osc::message(address,
                                       std::forward<ArgTypes>(args) ...));
            }
            
            void send(const std::string &address,
                      bbb::osc::message mess)
            {
                mess.address = address;
                send(mess);
            }
            
            void send(const bbb::osc::message &mess)
            {
                std::uint8_t buffer[bbb::udp::buf_size]{0};
                auto packet_size = mess.packet(buffer, bbb::udp::buf_size);
                this->udp::sender::send(buffer, packet_size);
            }
        }; // struct sender
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_sender_hpp */
