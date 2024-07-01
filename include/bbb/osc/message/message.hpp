/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/message/message.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_message_message_hpp
#define bbb_osc_message_message_hpp

#include <bbb/osc/message/convert/from_osc.hpp>
#include <bbb/osc/message/convert/to_osc.hpp>

#include <bbb/osc/argument.hpp>
#include <bbb/osc/common.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace bbb {
    namespace osc {
        namespace detail {
            template <typename t>
            struct remove_cr 
            { using type = t; };

            template <typename t>
            struct remove_cr<const t>
            { using type = t; };

            template <typename t>
            struct remove_cr<t &>
            { using type = t; };

            template <typename t>
            struct remove_cr<const t &>
            { using type = t; };

            template <typename t>
            struct remove_cr<t &&>
            { using type = t; };

            template <typename type_raw, typename type = typename remove_cr<type_raw>::type>
            struct is_osc_atomic_type 
            : std::integral_constant<
                bool,
                std::is_arithmetic<type>::value 
                || std::is_same<type, bool>::value 
                || std::is_same<type, std::string>::value
                || std::is_same<type, char *>::value // TODO: remove const reference etc
            > {};
        };

        struct message {
            std::string address;
            
            std::string binded_ip;
            std::uint16_t waiting_port;
            std::string received_host;
            std::uint16_t received_port;

            message() {};
            
            message(const std::string &address)
            : address(address) {}
            
            template <typename ArgumentType, typename ... ArgumentTypes>
            message(const std::string &address,
                    ArgumentType &&argument,
                    ArgumentTypes && ... arguments)
            : address(address)
            {
                push_list(std::forward<ArgumentType>(argument),
                          std::forward<ArgumentTypes>(arguments) ...);
            }
            
            void push(Tag tag) {
                args.emplace_back(tag);
            }

            template <typename ArgumentType>
            void push(Tag tag, ArgumentType arg) 
            {
                args.emplace_back(tag, arg);
            }
            
            void push(Tag tag, const void *buf, std::size_t size)
            {
                args.emplace_back(buf, size);
            }

            template <typename value_type>
            auto push(value_type v) 
                -> typename std::enable_if<
                    bbb::osc::detail::is_osc_atomic_type<value_type>::value
                >::type
            {
                args.emplace_back(v);
            }

            template <typename value_type>
            auto push(value_type &&v) 
                -> typename std::enable_if<
                    !bbb::osc::detail::is_osc_atomic_type<value_type>::value
                >::type
            {
                to_osc(*this, std::forward<value_type>(v));
            }

            // push blob
            void push(const void *buf, std::size_t size)
            {
                args.emplace_back(buf, size);
            }

//            // push timetag
//            void push(std::chrono::nanoseconds time)
//            {
//                args.emplace_back(arg);
//            }
            
            template <typename ArgumentType>
            void push_list(ArgumentType &&argument)
            {
                push(std::forward<ArgumentType>(argument));
            }

            template <typename ArgumentType, typename ... ArgumentTypes>
            void push_list(ArgumentType &&argument,
                           ArgumentTypes && ... arguments)
            {
                push(std::forward<ArgumentType>(argument));
                if(sizeof...(arguments)) {
                    push_list(std::forward<ArgumentTypes>(arguments) ...);
                }
            }
            
            template <typename ArgumentType>
            message &operator<<(ArgumentType &&arg) {
                push(std::forward<ArgumentType>(arg));
                return *this;
            }
            
            inline std::size_t size() const {
                return args.size();
            }
            inline argument operator[](std::size_t index) {
                return args[index];
            }
            inline const argument &operator[](std::size_t index) const {
                return args[index];
            }
            
            using iterator = std::vector<argument>::iterator;
            using const_iterator = std::vector<argument>::const_iterator;
            
            iterator begin() { return args.begin(); }
            iterator end() { return args.end(); }
            const_iterator begin() const { return args.cbegin(); }
            const_iterator end() const { return args.cend(); }
            const_iterator cbegin() const { return args.cbegin(); }
            const_iterator cend() const { return args.cend(); }
            
            using reverse_iterator = std::vector<argument>::reverse_iterator;
            using const_reverse_iterator = std::vector<argument>::const_reverse_iterator;
            
            reverse_iterator rbegin() { return args.rbegin(); }
            reverse_iterator rend() { return args.rend(); }
            const_reverse_iterator rbegin() const { return args.crbegin(); }
            const_reverse_iterator rend() const { return args.crend(); }
            const_reverse_iterator crbegin() const { return args.crbegin(); }
            const_reverse_iterator crend() const { return args.crend(); }
            
            std::size_t packet(void *buffer, size_t size) const
            {
                OSCPP::Client::Packet packet(buffer, size);
                packet.openMessage(address.c_str(),
                                   args.size());
                for(const auto &arg : args) {
                    arg.addToPacket(packet);
                }
                packet.closeMessage();
                return packet.size();
            }
            
            inline friend std::ostream &operator<<(std::ostream &os, const message &mess)
            {
                if(!mess.size()) return os << mess.address << " :[]";
                os << mess.address << " :[" << std::endl;
                for(std::size_t i = 0; i < mess.size(); i++) {
                    os << "  " << i << ": " << mess[i].stringify() << std::endl;
                }
                return os << "]";
            }

            template <typename value_type>
            value_type get(std::size_t offset = 0) const {
                value_type v;
                from_osc(*this, v, offset);
                return std::move(v);
            }

            template <typename argument_type, typename ... remaining_types>
            std::size_t to(argument_type &arg,
                           remaining_types & ... args) const
            {
                return to_impl(0, arg, args ...);
            }

        private:
            template <typename argument_type, typename ... remaining_types>
            std::size_t to_impl(std::size_t offset,
                                argument_type &arg,
                                remaining_types & ... args) const
            {
                offset += from_osc(*this, arg, offset);
                return offset + to_impl(offset, args ...);
            }
            template <typename argument_type>
            std::size_t to_impl(std::size_t offset,
                                argument_type &arg) const
            {
                return from_osc(*this, arg, offset);
            }

            std::vector<argument> args;
        }; // struct message

        template <typename osc_message>
        inline std::size_t from_osc(const osc_message &m,
                                    osc_message &v,
                                    std::size_t offset = 0)
        {
            v = m;
            return 0;
        }

        template <typename osc_message>
        inline std::size_t to_osc(osc_message &m,
                                  const osc_message &v)
        {
            m = v;
            return v.size();
        }
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_message_hpp */
