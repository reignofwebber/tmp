#include "util.h"

// std
#include <sstream>
#include <iomanip>
#include <stdexcept>


namespace zfutils {

std::string fromHexString(const std::string &src) {
    std::string dst;
    if (src.size() % 2) {
        log_error << "src.size % 2 != 0";
        return dst;
    }

    try {
        for (auto i = 0; i < src.size(); i += 2) {
            auto hex_number = src.substr(i, 2);
            std::size_t process_num;
            char number = std::stoi(hex_number, &process_num, 16);
            if (process_num != 2)
                throw std::invalid_argument("");
            dst += number;
        }
    } catch(const std::invalid_argument& e) {
        log_fatal << "can not cast to number";
        dst = "";
    }

    return dst;
}  // fromHexString

std::string toHexString(const std::string &src, bool prefix) {
    std::stringstream ss;
    for (const auto &c : src) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c & 0xFF);
    }
    if (prefix) {
        auto s = ss.str();
        if (s.empty()) {
            return "";
        } else {
            return std::string("0x") + s;
        }
    } else {
        return ss.str();
    }
}  // toHexString

// HexStream
HexStream::HexStream(const std::string &s) {
    this->s_ = s;
}

std::string HexStream::str() const {
    return s_;
}

HexStream &HexStream::operator<<(const char* s) {
    s_ += s;
    return *this;
}

HexStream &HexStream::string(std::string &s, std::size_t size) {
    if (s_.size() < size) {
        log_warning << "stream size " << s_.size() << " < " << "require size " << size;
        s = "";
        s_.clear();
        return *this;
    }
    s = s_.substr(0, size);
    s_ = s_.substr(size);
    return *this;
}

HexStream &HexStream::skip(std::size_t size) {
    if (s_.size() < size) {
        log_warning << "stream size " << s_.size() << " < " << "skip size " << size;
        s_.clear();
        return *this;
    }
    s_ = s_.substr(size);
    return *this;
}

bool HexStream::empty() const {
    return s_.empty();
}









}  // namespace zfutils


