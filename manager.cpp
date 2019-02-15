#include "manager.h"

// std
#include <string>
#include <utility>
#include <algorithm>
#include <exception>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// local
#include "log.h"
#include "message.h"


void Manager::start() {
    initPIO();
}

void Manager::welcome() {
    // pio config
    add_to_send_queue(m_pioConfig);
}

void Manager::updatePIOStatus(const std::string &ci, const std::string pio_id, bool status) {
    try {
        auto &pioInfo = m_pioConfig.pioList[ci][pio_id];
        if (pioInfo.status != status) {
            pioInfo.status = status;

            add_to_send_queue(pioInfo);
        }
    } catch (const std::exception &e) {
        log_error << e.what();
    }
}

void Manager::add_to_send_queue(const MessageObject &mobj) {
    log_info << mobj.msg();
}

void Manager::initPIO() {
    using boost::property_tree::ptree;
    ptree pt;
    boost::property_tree::read_xml("config/pio_config.xml", pt);
    for (auto &v : pt.get_child("CILIST")) {
        auto pt_ci = v.second;
        std::string ci = pt_ci.get<std::string>("<xmlattr>.id", "");
        if (ci.empty()) continue;

        // get pios
        for (auto &p : pt_ci) {
            PIOInfo info;
            info.ci = ci;
            info.id = p.second.get<std::string>("<xmlattr>.id", "");
            info.type = p.second.get<std::string>("<xmlattr>.type", "");
            m_pioConfig.pioList[ci][info.id] = info;
        }
    }
}
