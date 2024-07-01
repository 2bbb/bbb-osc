/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/argument.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_argument_hpp
#define bbb_osc_argument_hpp

#include <bbb/osc/common.hpp>

#include <iostream>

namespace bbb {
    namespace osc {
        struct midi {
            uint8_t port;
            uint8_t status;
            uint8_t data1;
            uint8_t data2;
        };
        struct color {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        struct argument {
            union number_value {
                bool b;
                std::int8_t c;
                std::int32_t i;
                std::uint32_t u;
                std::int64_t l;
                std::uint64_t ul;
                struct color color;
                struct midi midi;
                float f;
                double d;
            } num;
            
            std::vector<std::uint8_t> blob;
            
            const Tag tag;
            std::string str;

            argument(bool b)
            : tag(b ? Tag::True : Tag::False) { num.b = b; }

            argument(char c)
            : tag(Tag::Char) { num.c = c; }
            
            argument(signed char i)
            : tag(Tag::Int32) { num.i = i; }
            argument(unsigned char i)
            : tag(Tag::Int32) { num.i = i; }

            argument(short i)
            : tag(Tag::Int32) { num.i = i; }
            argument(unsigned short i)
            : tag(Tag::Int32) { num.i = i; }
            argument(int i)
            : tag(Tag::Int32) { num.i = i; }

            argument(unsigned int l)
            : tag(Tag::Int64) { num.l = l; }
            argument(long l)
            : tag(Tag::Int64) { num.l = l; }
            argument(unsigned long l)
            : tag(Tag::Int64) { num.l = l; }
            
            argument(float f)
            : tag(Tag::Float) { num.f = f; }
            argument(double d)
            : tag(Tag::Double) { num.d = d; }

            argument(const std::string &str)
            : tag(Tag::String), str(str) {}
            
            argument(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
            : tag(Tag::Midi4) {
                num.midi.port = byte1;
                num.midi.status = byte2;
                num.midi.data1 = byte3;
                num.midi.data2 = byte4;
            }
            
            argument(const void *buf, std::size_t size)
            : tag(Tag::Blob) {
                blob.clear();
                blob.resize(size);
                std::memcpy(blob.data(), buf, size);
            }
            
            argument(Tag tag)
            : tag(tag) {
                switch(static_cast<Tag>(tag)) {
                    case Tag::True:
                        num.b = true;
                        break;
                    case Tag::False:
                        num.b = false;
                        break;
                    case Tag::NIL:
                    case Tag::IMPULSE:
                        break;
                    default:
                        std::cerr << "bbb::osc::argument" << "invalid message: tag is " << tag << " [" << TagName(tag) << "]. but argument is not given." << std::endl;
                }
            }

            argument(Tag tag, std::int8_t c)
            : tag{tag} { num.c = c; }
            argument(Tag tag, std::int32_t i)
            : tag{tag} { num.i = i; }
            argument(Tag tag, std::int64_t l)
            : tag{tag} { num.l = l; }
            argument(Tag tag, float f)
            : tag{tag} { num.f = f; }
            argument(Tag tag, double d)
            : tag{tag} { num.d = d; }
            argument(Tag tag, const std::string &str)
            : tag{tag}, str(str) {}
            
#define cast_num(type, name) static_cast<type>(num.name)

#define define_num_cast_operator(type)\
            operator type() const {\
                switch(tag) {\
                    case OSCPP::Tag::True:\
                    case OSCPP::Tag::False:\
                        return cast_num(type, b);\
                    case OSCPP::Tag::Char:   return cast_num(type, c);\
                    case OSCPP::Tag::Int32:  return cast_num(type, i);\
                    case OSCPP::Tag::Int64:  return cast_num(type, l);\
                    case OSCPP::Tag::Float:  return cast_num(type, f);\
                    case OSCPP::Tag::Double: return cast_num(type, d);\
                    case OSCPP::Tag::String: return static_cast<type>(std::stol(str));\
                    case OSCPP::Tag::Midi4:  return cast_num(type, u);\
                    case OSCPP::Tag::RGBA:   return cast_num(type, u);\
                    case OSCPP::Tag::Timetag:   return cast_num(type, ul);\
                    default:\
                        std::cerr << "bbb::osc::Argument" << ("argument is not " #type ": ") << OSCPP::TagName(tag);\
                        return static_cast<type>(0);\
                }\
            }

            define_num_cast_operator(bool);
            define_num_cast_operator(char);
            define_num_cast_operator(signed char);
            define_num_cast_operator(unsigned char);
            define_num_cast_operator(short);
            define_num_cast_operator(unsigned short);
            define_num_cast_operator(int);
            define_num_cast_operator(unsigned int);
            define_num_cast_operator(long);
            define_num_cast_operator(unsigned long);
            define_num_cast_operator(long long);
            define_num_cast_operator(unsigned long long);
            define_num_cast_operator(float);
            define_num_cast_operator(double);
            
            operator midi() const {
                return num.midi;
            }
            
            operator color() const {
                return num.color;
            }
            
#undef cast_num
#undef define_num_cast_operator

            operator std::string() const {
                if(tag != OSCPP::Tag::String) {
                    std::cerr << "bbb::osc::argument" << ("argument is not string: ") << OSCPP::TagName(tag);
                }
                return stringify();
            }
            
            void addToPacket(OSCPP::Client::Packet &packet) const {
                switch(tag) {
                    case OSCPP::Tag::String:
                    case OSCPP::Tag::Symbol:
                        packet.string(str.c_str());
                        break;
                    case OSCPP::Tag::True:
                        packet.boolean(true);
                        break;
                    case OSCPP::Tag::False:
                        packet.boolean(false);
                        break;
                    case OSCPP::Tag::Char:
                        packet.character(num.c);
                        break;
                    case OSCPP::Tag::Int32:
                        packet.int32(num.i);
                        break;
                    case OSCPP::Tag::Int64:
                        packet.int64(num.l);
                        break;
                    case OSCPP::Tag::Float:
                        packet.float32(num.f);
                        break;
                    case OSCPP::Tag::Double:
                        packet.float64(num.d);
                        break;
                    case OSCPP::Tag::Blob:
                        packet.blob(OSCPP::Blob(blob.data(), blob.size()));
                        break;
                    case OSCPP::Tag::Midi4:
                        packet.midi(num.i);
                        break;
                    case OSCPP::Tag::RGBA:
                        packet.rgba(num.i);
                        break;
                    case OSCPP::Tag::IMPULSE:
                        packet.impulse();
                        break;
                    case OSCPP::Tag::NIL:
                        packet.NIL();
                        break;
                    case OSCPP::Tag::Timetag:
                        // TODO:
                        packet.timetag(num.ul);
                        break;
                }
            }
            
            inline std::string stringify() const {
                switch(tag) {
                    case OSCPP::Tag::String:
                    case OSCPP::Tag::Symbol:
                        return str;
                    case OSCPP::Tag::Char:
                        return std::to_string(num.c);
                    case OSCPP::Tag::Int32:
                        return std::to_string(num.i);
                    case OSCPP::Tag::Int64:
                        return std::to_string(num.l);
                    case OSCPP::Tag::Float:
                        return std::to_string(num.f);
                    case OSCPP::Tag::Double:
                        return std::to_string(num.d);
                    case OSCPP::Tag::Midi4:
                        return "midi("
                            + std::to_string(+num.midi.port) + ","
                            + std::to_string(+num.midi.status) + ","
                            + std::to_string(+num.midi.data1) + ","
                            + std::to_string(+num.midi.data2) +
                        ")";
                    case OSCPP::Tag::RGBA:
                        return "rgba("
                            + std::to_string(+num.color.r) + ","
                            + std::to_string(+num.color.g) + ","
                            + std::to_string(+num.color.b) + ","
                            + std::to_string(+num.color.a) +
                        ")";
                    default:
                        return TagName(static_cast<char>(tag));
                }
            }
            
            template <typename type>
            type get() const {
                return operator type();
            }
            
            inline friend std::ostream &operator<<(std::ostream &os, const bbb::osc::argument &arg) {
                return os << arg.tag << ": " << arg.stringify();
            }
        }; // struct argument
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_argument_hpp */
