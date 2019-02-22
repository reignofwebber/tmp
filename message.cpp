#include "message.h"

// std
#include <algorithm>
#include <unordered_map>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>

// local
#include "log.h"


void Message::treeFromData(const std::string &root) {
    // id and type attr
    if (root == "message.") {
        pt_.put(root + "<xmlattr>.id", id_);
        pt_.put(root + "<xmlattr>.type", type_);
    }

    // strings
    std::string strings_prefix(root + "strings.");
    std::for_each(strings_.begin(), strings_.end(),
        [this, &strings_prefix](const std::unordered_map<std::string, std::string>::value_type &val) {
            pt_.put(strings_prefix + val.first, val.second);
        });

    // arrays
    std::string arrays_prefix(root + "arrays.");
    std::for_each(arrays_.begin(), arrays_.end(),
        [this, &arrays_prefix](const std::unordered_map<std::string, std::vector<std::string>>::value_type &val) {
            // std::string arr_strings;
            // std::for_each(val.second.begin(), val.second.end(), [&](const std::string &str)
            // {
            //     arr_strings += str;
            //     arr_strings.push_back(',');
            // });
            // arr_strings.pop_back();

            pt_.put(arrays_prefix + val.first, boost::join(val.second, ","));
        });

    // packets
    std::string packets_prefix(root + "packets.");
    std::for_each(packets_.begin(), packets_.end(),
        [this, &packets_prefix](const std::pair<std::string, Message> &val) {
            Message subMsg = val.second;
            subMsg.treeFromData("");
            pt_.add_child(packets_prefix + val.first, subMsg.pt_);
        });

    // packetArrays
    std::string packetArrays_prefix(root + "packetArrays.");
    std::for_each(packetArrays_.begin(), packetArrays_.end(),
        [this, &packetArrays_prefix](const std::pair<std::string, std::vector<Message>> &val) {
            std::vector<Message> subMsgs = val.second;
            ptree tmptree;
            for (auto &subMsg : subMsgs) {
                subMsg.treeFromData("");
                tmptree.push_back(std::make_pair("packet", subMsg.pt_));
            }
            pt_.add_child(packetArrays_prefix + val.first, tmptree);
        });
}

void Message::treeToData(const std::string &xmlData, const std::string &root) {
    std::stringstream s(xmlData);
    try {
        boost::property_tree::read_xml(s, pt_);
    } catch (const boost::property_tree::xml_parser_error &xe) {
        log_error << "parse xml error , xml is [" << xmlData << "]";
        no_error_ = false;
        return;
    }

    // id and type attr
    if (root == "message.") {
        id_ = pt_.get<std::string>("message.<xmlattr>.id", "");
        type_ = pt_.get<std::string>("message.<xmlattr>.type", "");
    }

    // strings
    auto strings = pt_.get_child_optional(root + "strings");
    if (strings) {
        for (const auto &val : strings.get()) {
            strings_[val.first] = val.second.data();
        }
    }

    // arrays
    auto arrays = pt_.get_child_optional(root + "arrays");
    if (arrays) {
        for (const auto &val : arrays.get()) {
            std::vector<std::string> strings;
            boost::split(strings, val.second.data(), [](char c){return c == ',';});
            arrays_[val.first] = strings;
        }
    }

    // packets
    auto packets = pt_.get_child_optional(root + "packets");
    if (packets) {
        for (const auto &val : packets.get()) {
            Message subMsg;
            std::cout << "test..." << val.first << " " << val.second.data() << std::endl;
            subMsg.treeToData(xmlData, root + "packets." + val.first + ".");
            packets_[val.first] = subMsg;
        }
    }
}
