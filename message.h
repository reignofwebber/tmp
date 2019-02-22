#ifndef MESSAGE_H_
#define MESSAGE_H_

// std
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <utility>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/endian/conversion.hpp>

// local
#include "log.h"
#include "util.h"

#if defined(DEBUG)

// #include "../settings.hpp"

#endif  // DEBUG



using boost::property_tree::ptree;

class Message {
 public:
    Message() : has_tree_(false), no_error_(true) {}
    explicit Message(const std::string &xmlData)
        : no_error_(true) {
        treeToData(xmlData);
    }

    ~Message() {}

    static std::string msg(const std::string &type, const std::string &id,
                        const std::unordered_map<std::string, std::string> &strings,
                        const std::unordered_map<std::string, std::vector<std::string>> &arrays) {
                            Message msg;
                            msg.setId(id);
                            msg.setType(type);
                            msg.setStrings(strings);
                            msg.setArrays(arrays);
                            return msg.toXmlWithLength();
                        }

    ////////////////////////////////////////////////////////////////////////////////////////
    // data --> xml
    std::string toXml() {
        if (!has_tree_)
            treeFromData();

        has_tree_ = true;
        std::stringstream s;
        using boost::property_tree::xml_parser::xml_writer_settings;
        boost::property_tree::write_xml(s, pt_, xml_writer_settings<std::string>());

#if defined(DEBUG)
        // Settings &settings = Settings::getInstance();
        // std::string tmp = s.str();
        // if (settings.debug_xml_msg_type == "") {
        //     log_trace << tmp;
        // } else if (settings.debug_xml_msg_type == type_ && settings.debug_xml_msg_id == "") {
        //     log_trace << tmp;
        // } else if (settings.debug_xml_msg_type == type_ && settings.debug_xml_msg_id == id_) {
        //     log_trace << tmp;
        // }
        log_trace << s.str();
#endif  // DEBUG
        return s.str();
    }

    std::string toXmlWithLength() {
        auto s = toXml();
        std::string msg_size = zfutils::dump_hex<uint32_t>(s.size());
        return msg_size + s;
    }

    // id and type
    void setId(const std::string& id) {
        id_ = id;
    }

    void setType(const std::string& type) {
        type_ = type;
    }

    // add methods
    void addString(const std::string &fir, const std::string &snd) {
        strings_[fir] = snd;
    }

    void addString(const std::string &fir, int64_t snd) {
        strings_[fir] = std::to_string(snd);
    }

    void addString(const std::string &fir, uint64_t snd) {
        strings_[fir] = std::to_string(snd);
    }

    void addArray(const std::string &key, const std::vector<std::string> &val) {
        arrays_[key] = val;
    }

    void appendArray(const std::string &key, const std::string &val) {
        arrays_[key].push_back(val);
    }

    void addPacket(const std::string &key, const Message &msg) {
        packets_[key] = msg;
    }

    void addPacketArray(const std::string &key, const std::vector<Message> &packetArray) {
        packetArrays_[key] = packetArray;
    }

    // set methods
    void setStrings(const std::unordered_map<std::string, std::string> &strings) {
        strings_ = strings;
    }

    void setArrays(const std::unordered_map<std::string, std::vector<std::string>> &arrays) {
        arrays_ = arrays;
    }

    void setPackets(const std::map<std::string, Message> &packets) {
        packets_ = packets;
    }

    void setPacketArrays(const std::string &key, const std::map<std::string, std::vector<Message>> &packetArrays) {
        packetArrays_ = packetArrays;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // xml --> data  ---> use constructor replace
    // void fromXml(const std::string &xml)
    // {

    // }

    // id and type
    std::string getId() const {
        return id_;
    }

    std::string getType() const {
        return type_;
    }

    std::string getString(const std::string &s) const {
        if (strings_.find(s) == strings_.end()) {
            log_error << "no string named " << s;
            return "";
        }
        return strings_.at(s);
    }

    std::unordered_map<std::string, std::string> getStrings() const {
        return strings_;
    }

    std::vector<std::string> getArray(const std::string &s) const {
        if (arrays_.find(s) == arrays_.end()) {
            log_error << "no string named " << s;
            return {};
        }
        return arrays_.at(s);
    }

    std::unordered_map<std::string, std::vector<std::string>> getArrays() const {
        return arrays_;
    }

    std::map<std::string, Message> getPackets() const {
        return packets_;
    }

    std::pair<std::string, std::string> getIdentity() const {
        return {type_, id_};
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////
    // reset data
    void reset() {
        has_tree_ = false;
        id_ = "";
        type_ = "";
        strings_.clear();
        arrays_.clear();
        packets_.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // parse is ok?
    bool has_error() {
        return !no_error_;
    }

 private:
    void treeFromData(const std::string &root = "message.");
    void treeToData(const std::string &xmlData, const std::string &root = "message.");

 private:
    ptree pt_;
    bool has_tree_;
    bool no_error_;

    std::string id_;
    std::string type_;
    std::unordered_map<std::string, std::string> strings_;
    std::unordered_map<std::string, std::vector<std::string>> arrays_;
    std::map<std::string, Message> packets_;
    std::map<std::string, std::vector<Message>> packetArrays_;
};





#endif  // MESSAGE_H_
