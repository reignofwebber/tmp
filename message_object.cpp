#include "message_object.h"

// std
#include <utility>
#include <algorithm>

// local
#include "message.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C2S

MessageObject getMessageObject(const std::string &msg) {
    Message xmsg(msg);
    if (xmsg.getType() == )
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// control protocol
std::string C2SAuthentication::msg() const {
    
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string PIOInfo::msg() const {
    Message msg;
    msg.setType("UI_CIM");
    // in or out
    int pos = 0;
    if (id.find("In") != std::string::npos) {
        msg.setId("et_in");
        pos = std::stoi(id.substr(2));
    } else {
        msg.setId("et_out");
        pos = std::stoi(id.substr(3));
    }
    // ci
    msg.addString("ci", ci);
    // pos
    msg.addString("pos", std::to_string(pos));
    // status
    if (status) {
        msg.addString("status", "true");
    } else {
        msg.addString("status", "false");
    }

    return msg.toXmlWithLength();
}

std::string PIOConfig::msg() const {
    std::string msgs;
    for (const auto &v : pioList) {
        std::vector<std::string> pio_id_v;
        std::vector<std::string> pio_type_v;

        auto &pioMap = v.second;
        std::transform(pioMap.begin(), pioMap.end(), std::back_inserter(pio_id_v),
            [](const std::pair<std::string, PIOInfo> &p) {
                return p.first;
            });
        std::transform(pioMap.begin(), pioMap.end(), std::back_inserter(pio_type_v),
            [](const std::pair<std::string, PIOInfo> &p) {
                return p.second.type;
            });

        msgs += Message::msg("UI_CIM", "et_config", {
            {"ci", v.first}
        }, {
            {"index", pio_id_v},
            {"name", pio_id_v},
            {"type", pio_type_v}
        }).toXmlWithLength();
    }
    return msgs;
}