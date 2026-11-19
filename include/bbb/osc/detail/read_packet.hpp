#pragma once

#include <bbb/osc/message.hpp>

namespace bbb {
namespace osc {
namespace detail {

// Walk the tag stream independently: every iteration consumes exactly one tag,
// including atoms without payload. Unknown payload widths cannot be skipped.
inline bool read_arguments(const OSCPP::Server::ArgStream &arguments, message &result) {
    auto streams = arguments.state();
    auto &tags = std::get<0>(streams);
    auto &values = std::get<1>(streams);
    while(!tags.atEnd()) {
        const auto tag = static_cast<Tag>(tags.getChar());
        switch(tag) {
            case Tag::True: case Tag::False: case Tag::IMPULSE: case Tag::NIL:
                result.push(tag);
                break;
            case Tag::Char:
                result.push(tag, static_cast<std::int8_t>(values.getInt32()));
                break;
            case Tag::Int32: case Tag::Midi4: case Tag::RGBA:
                result.push(tag, values.getInt32());
                break;
            case Tag::Int64: result.push(tag, values.getInt64()); break;
            case Tag::Timetag: result.push(tag, values.getUInt64()); break;
            case Tag::Float: result.push(tag, values.getFloat32()); break;
            case Tag::Double: result.push(tag, values.getFloat64()); break;
            case Tag::String: case Tag::Symbol:
                result.push(tag, std::string(values.getString()));
                break;
            case Tag::Blob: {
                const auto length = values.getInt32();
                if(length < 0) return false;
                const auto data = values.pos();
                values.skip(OSCPP::align(static_cast<std::size_t>(length)));
                result.push(tag, data, static_cast<std::size_t>(length));
                break;
            }
            default: return false;
        }
    }
    return true;
}

constexpr unsigned max_bundle_depth = 32;

template <typename Deliver>
void read_packet(const OSCPP::Server::Packet &packet, const Deliver &deliver, unsigned depth = 0) {
    if(packet.isBundle()) {
        if(max_bundle_depth <= depth) return;
        OSCPP::Server::Bundle bundle(packet);
        auto packets = bundle.packets();
        while(!packets.atEnd()) read_packet(packets.next(), deliver, depth + 1);
        return;
    }
    OSCPP::Server::Message input(packet);
    message result(input.address());
    if(read_arguments(input.args(), result)) deliver(result);
}

} // namespace detail
} // namespace osc
} // namespace bbb
