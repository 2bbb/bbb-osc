//
//  bbb/osc/argument.hpp
//
//  Created by 2bit on 2022/08/22.
//

#ifndef bbb_osc_argument_hpp
#define bbb_osc_argument_hpp

#include <bbb/osc/common.hpp>

#include <iostream>

namespace bbb {
    namespace osc {
        struct argument {
            union number_value {
                bool b;
                std::int8_t c;
                std::int32_t i;
                std::int64_t l;
                float f;
                double d;
            } num;
            std::string str;
            
            const TagType tag;
            
            argument(bool b)
            : tag(b ? Tag::True : Tag::False) { num.b = b; }
            argument(std::int8_t c)
            : tag(Tag::Char) { num.c = c; }
            
            argument(std::uint8_t i)
            : tag(Tag::Int32) { num.i = i; }
            argument(std::int16_t i)
            : tag(Tag::Int32) { num.i = i; }
            argument(std::uint16_t i)
            : tag(Tag::Int32) { num.i = i; }
            argument(std::int32_t i)
            : tag(Tag::Int32) { num.i = i; }

            argument(std::uint32_t l)
            : tag(Tag::Int64) { num.l = l; }
            argument(std::int64_t l)
            : tag(Tag::Int64) { num.l = l; }
            argument(std::uint64_t l)
            : tag(Tag::Int64) { num.l = l; }
            
            argument(float f)
            : tag(Tag::Float) { num.f = f; }
            argument(double d)
            : tag(Tag::Double) { num.d = d; }

            argument(const std::string &str)
            : tag(Tag::String), str(str) {}
            
            argument(TagType tag)
            : tag(tag) {
                switch(tag) {
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
                        std::cerr << "bbb::osc::argument" << "invalid message: tag is " << tag << " [" << TagName(tag) << "]. but argument is not given.";
                }
            }

            argument(TagType tag, std::int8_t c)
            : tag(tag) { num.c = c; }
            argument(TagType tag, std::int32_t i)
            : tag(tag) { num.i = i; }
            argument(TagType tag, std::int64_t l)
            : tag(tag) { num.l = l; }
            argument(TagType tag, float f)
            : tag(tag) { num.f = f; }
            argument(TagType tag, double d)
            : tag(tag) { num.d = d; }
            argument(TagType tag, const std::string &str)
            : tag(tag), str(str) {}
            
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
            define_num_cast_operator(float);
            define_num_cast_operator(double);

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
                    case OSCPP::Tag::True: // TODO
                        packet.boolean(true);
                        break;
                    case OSCPP::Tag::False: // TODO
                        packet.boolean(false);
                        break;
                    case OSCPP::Tag::Char: // TODO
                        packet.character(num.c);
                        break;
                    case OSCPP::Tag::Int32:
                        packet.int32(num.i);
                        break;
                    case OSCPP::Tag::Int64: // TODO
                        packet.int64(num.l);
                        break;
                    case OSCPP::Tag::Float:
                        packet.float32(num.f);
                        break;
                    case OSCPP::Tag::Double: // TODO
                        packet.float64(num.d); // TODO
                        break;
                    default:
                        // TODO Midi4, NIL, IMPULSE, Timetag
                        break;
                }
            }
            
            inline std::string stringify() const {
                switch(tag) {
                    case OSCPP::Tag::String:
                    case OSCPP::Tag::Symbol:
                        return str;
                    case OSCPP::Tag::True:
                        return std::string("True");
                    case OSCPP::Tag::False:
                        return std::string("False");
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
                    default:
                        return TagName(tag);
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
