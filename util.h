#ifndef UTIL_H_
#define UTIL_H_

// std
#include <string>
#include <algorithm>

// boost
#include <boost/noncopyable.hpp>

// local
#include "log.h"

namespace zfutils {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// binary stream part
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// s = fromHexString("414243") s = "ABC"
std::string fromHexString(const std::string &);

// s = toHexString("ABC") s = "414243"
std::string toHexString(const std::string &, bool prefix = false);

template <class T>
T hex_cast(const std::string &src, bool disable_size_comp = false) {
    if (!disable_size_comp &src.size() > sizeof(T)) {
        log_warning << "arrow conversion: src: " << src.size() << ", but dst: " << sizeof(T);
    }
    int length = std::min(src.size(), sizeof(T));
    // left shift pos
    int pos = 0;
    T t = 0;
    for (const auto &c : src) {
        auto shift_num = (length - 1 - pos) * 8;
        t += ((uint8_t)c << shift_num);
        ++pos;
        if (pos == length) {
            break;
        }
    }
    return t;
}  // hex_cast



template <class T>
std::string dump_hex(const T &t) {
    std::string s;
    int length = sizeof(T);
    while (length) {
        auto shift_num = (length - 1) * 8;
        auto c = static_cast<char>(t >> shift_num);
        s += c;
        --length;
    }
    return s;
}

    // HexStream
class HexStream : private boost::noncopyable {
 public:
    HexStream() = default;
    explicit HexStream(const std::string &s);

        // <<
    template <class T>
    HexStream &operator<<(const T &number) {
        s_ += dump_hex<T>(number);
        return *this;
    }

    HexStream &operator<<(const char *);

    // >>
    template <class T>
    HexStream &operator>>(T &number) {
        number = hex_cast<T>(s_, true);
        if (s_.size() < sizeof(T)) {
            s_.clear();
            return *this;
        }
        s_ = s_.substr(sizeof(T));
        return *this;
    }

    // get string from stream
    HexStream &string(std::string &s, std::size_t size);
    // skip bytes from stream
    HexStream &skip(std::size_t size);

    // if stream is empty
    bool empty() const;

    // retrieve string from stream
    std::string str() const;

 private:
    std::string s_;
};

template <>
inline HexStream &HexStream::operator<<(const std::string &s) {
    s_ += s;
    return *this;
}
// !HexStream

// BinaryStream
// for fixed length and formatted string
class BinaryStream : private boost::noncopyable {
 public:
    BinaryStream() = default;

    explicit BinaryStream(const std::string &s) : s_(s) {}

    // get from stream (byteoffset, bitoffset(low), length(bit length))
    template <class T>
    T get(std::size_t byteoffset, int bitoffset = 7, std::size_t length = sizeof(T) * 8) {
        std::size_t t_size = sizeof(T) * 8;
        if (length > t_size) {
            log_warning << "narrow conversion, type's length is " << t_size << ", but require length is " << length;
        }

        if (byteoffset >= s_.size() || bitoffset > 7) {
            log_fatal << "can not get number due to invalid argument";
            return 0;
        }

        T res = 0;
        length = std::min(length, t_size);

        while (true) {
            auto c = s_.at(byteoffset);
            auto bit = (c >> bitoffset) & 1;
            res += (bit << (length - 1));
            if (!--length) {
                break;
            }
            --bitoffset;
            if (bitoffset < 0) {
                ++byteoffset;
                if (byteoffset == s_.size()) {
                    log_fatal << "can not get number due to invalid argument";
                    return 0;
                }
                bitoffset = 7;
            }
        }
        return res;
    }

    template <class T>
    bool set(T t, std::size_t byteoffset, int bitoffset, std::size_t length) {
    }

 private:
    std::string s_;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// measure part
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



}  // namespace zfutils




#endif  // UTIL_H_
