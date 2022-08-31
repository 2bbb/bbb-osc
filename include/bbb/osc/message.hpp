//
//  bbb/osc/message.hpp
//
//  Created by 2bit on 2022/08/22.
//

#ifndef bbb_osc_message_hpp
#define bbb_osc_message_hpp

#include <bbb/osc/argument.hpp>
#include <bbb/osc/common.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace bbb {
    namespace osc {
        class message {
            std::vector<argument> args;
        public:
            std::string address;
            message() {};
            
            message(const std::string &address)
            : address(address) {}
            
            template <typename ArgumentType, typename ... ArgumentTypes>
            message(const std::string &address,
                    ArgumentType &&argument,
                    ArgumentTypes && ... arguments)
            : address(address)
            {
                push(std::forward<ArgumentType>(argument),
                     std::forward<ArgumentTypes>(arguments) ...);
            }
            
            void push(TagType tag) {
                args.emplace_back(tag);
            }

            template <typename ArgumentType>
            void push(TagType tag, ArgumentType &&arg) {
                args.emplace_back(tag, std::forward<ArgumentType>(arg));
            }
            
            template <typename ArgumentType>
            void push_list(ArgumentType &&argument)
            {
                args.emplace_back(std::forward<ArgumentType>(argument));
            }

            template <typename ArgumentType, typename ... ArgumentTypes>
            void push_list(ArgumentType &&argument,
                              ArgumentTypes && ... arguments)
            {
                args.emplace_back(std::forward<ArgumentType>(argument));
                if(sizeof...(arguments)) {
                    push_list(std::forward<ArgumentTypes>(arguments) ...);
                }
            }
            
            template <typename ArgumentType>
            message &operator<<(ArgumentType &&arg) {
                args.emplace_back(std::forward<ArgumentType>(arg));
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
        }; // struct message
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_hpp */
