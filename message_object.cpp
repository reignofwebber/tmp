#include "message_object.h"

// std
#include <utility>
#include <algorithm>

// boost
#include <boost/lexical_cast.hpp>

// local
#include "message.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageObject> getMessageObject(const std::string &msg) {
    Message xmsg(msg);
    auto type = xmsg.getType();
    auto id = xmsg.getId();
    if (type == "UI_AUTHENTICATION") {
        return std::make_shared<C2SAuthentication>(xmsg);
    } else if (type == "UI_CONFIGURATION") {
        return std::make_shared<C2SConfiguration>(xmsg);
    } else if (type == "UI_SUBSCRIPTION") {
        return std::make_shared<C2SSubscription>(xmsg);
    } else if (type == "UI_PLAYBACK") {
        if (id == "start") {
            return std::make_shared<C2SPlayback_Start>(xmsg);
        } else if (id == "option") {
            return std::make_shared<C2SPlayback_Option>(xmsg);
        } else {
            return std::make_shared<C2SPlayback_Control>(xmsg);
        }
    } else if (type == "UI_HEARTBEAT") {
        return std::make_shared<C2SHeartBeat>();
    } else if (type == "UI_CLOSE") {
        return std::make_shared<C2SClose>();
    }
    log_error << "unimplement client msg type!!";
    return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 控制协议
// Authentication
C2SAuthentication::C2SAuthentication(const Message &msg) {
    username = msg.getString("username");
    password = msg.getString("password");
}

std::string S2CAuthenticationReply::msg() const {
    if (success) {
        return Message::msg("UI_AUTHENTICATION", "accept", {}, {});
    } else {
        return Message::msg("UI_AUTHENTICATION", "refuse", {
            {"reason", reason}
        }, {});
    }
}

// Configuration
C2SConfiguration::C2SConfiguration(const Message &msg) {
    softwareVersion = msg.getString("softwareVersion");
    dataVersion = msg.getString("dataVersion");
}

std::string S2CConfigurationReply::msg() const {
    if (success) {
        return Message::msg("UI_CONFIGURATION", "accept", {}, {});
    } else {
        return Message::msg("UI_CONFIGURATION", "refuse", {}, {});
    }
}

// Subscription
C2SSubscription::C2SSubscription(const Message &msg) {
    if (msg.getType() == "UI_SUBSCRIPTION") {
        subscription = true;
    } else {
        subscription = false;
    }
    if (msg.getId() == "realtime") {
        type = RealTime_Type;
    } else {
        type = History_Type;
    }
    // TODO(zf) ruleset

    auto level_s = msg.getString("level");
    if (level_s == "AREA") {
        level = Subscribe_Area;
    } else if (level_s == "LINE") {
        level = Subscribe_Line;
    } else {
        level = Subscribe_Device;
    }
    id = msg.getString("id");
}

// playback
C2SPlayback_Start::C2SPlayback_Start(const Message &msg) {
    startTime = boost::lexical_cast<uint64_t>(msg.getString("startTime"));
    endTime = boost::lexical_cast<uint64_t>(msg.getString("endTime"));
    rate = boost::lexical_cast<float>(msg.getString("rate"));
}

C2SPlayback_Option::C2SPlayback_Option(const Message &msg) {
    rate = boost::lexical_cast<float>(msg.getString("rate"));
}

C2SPlayback_Control::C2SPlayback_Control(const Message &msg) {
    auto id = msg.getId();
    if (id == "pause") {
        control = Playback_Pause;
    } else if (id == "resume") {
        control = Playback_Resume;
    } else {
        control = Playback_Stop;
    }
}

std::string S2CPlayback_Time::msg() const {
    return Message::msg("UI_PLAYBACK", "time", {
        {"currentTime", std::to_string(curTime)}
    }, {});
}

// heartbeat
std::string S2CHeartBeat::msg() const {
    return Message::msg("UI_HEARTBEAT", "", {}, {});
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 查询
C2SQueryBase::C2SQueryBase(const Message &msg) {
    id = msg.getString("id");
    db = msg.getString("db");
    startTime = boost::lexical_cast<uint64_t>(msg.getString("startTime"));
    endTime = boost::lexical_cast<uint64_t>(msg.getString("endTime"));
}

C2SQuerySelect::C2SQuerySelect(const Message &msg)
    : C2SQueryBase(msg) {
}

C2SQueryCount::C2SQueryCount(const Message &msg)
    : C2SQueryBase(msg) {
}

C2SQueryDistinct::C2SQueryDistinct(const Message &msg)
    : C2SQueryBase(msg) {
    key = msg.getString("key");
}

C2SQueryAverage::C2SQueryAverage(const Message &msg)
    : C2SQueryDistinct(msg) {
}

C2SQueryMax::C2SQueryMax(const Message &msg)
    : C2SQueryDistinct(msg) {
}

C2SQueryMin::C2SQueryMin(const Message &msg)
    : C2SQueryDistinct(msg) {
}

std::string S2CQuerySelectResult::msg() const {
    return Message::msg("UI_QUERY_RESULT", "select", {
        {"id", id}
    }, {});
}

std::string S2CQueryCountResult::msg() const {
    return Message::msg("UI_QUERY_RESULT", "count", {
        {"id", id},
        {"result", std::to_string(result)}
    }, {});
}

std::string S2CQueryDistinctResult::msg() const {
    return Message::msg("UI_QUERY_RESULT", "distinct", {
        {"id", id}
    }, {});
}

std::string S2CQueryAverageResult::msg() const {
    return Message::msg("UI_QUERY_RESULT", "average", {
        {"id", id},
        {"result", std::to_string(result)}
    }, {});
}

std::string S2CQueryMaxResult::msg() const {
    return Message::msg("UI_QUERY_RESULT", "max", {
        {"id", id},
        {"result", std::to_string(result)}
    }, {});
}

std::string S2CQueryMinResult::msg() const {
    return Message::msg("UI_QUERY_RESULT", "min", {
        {"id", id},
        {"result", std::to_string(result)}
    }, {});
}

C2SStatisticsDevice::C2SStatisticsDevice(const Message &msg) {
    id = msg.getString("id");
    auto array = msg.getArray("timeUnits");
    std::transform(array.begin(), array.end(), std::back_inserter(timeUnits), [](const std::string &s) {
        auto pos = s.find("-");
        uint64_t start{0}, end{0};
        if (pos != std::string::npos && pos != 0) {
            auto start = boost::lexical_cast<uint64_t>(s.substr(0, pos));
            auto end = boost::lexical_cast<uint64_t>(s.substr(pos + 1));
        }
        return std::make_pair(start, end);
    });
}

std::string S2CStatisticsDeviceResult::msg() const {
    return Message::msg("UI_STATISTICS_RESULT", "deviceAlarm", {
        {"id", id}
    }, {});
}

C2SStatisticsStation::C2SStatisticsStation(const Message &msg) {
    id = msg.getString("id");
    auto array = msg.getArray("timeUnits");
    std::transform(array.begin(), array.end(), std::back_inserter(timeUnits), [](const std::string &s) {
        auto pos = s.find("-");
        uint64_t start{0}, end{0};
        if (pos != std::string::npos && pos != 0) {
            auto start = boost::lexical_cast<uint64_t>(s.substr(0, pos));
            auto end = boost::lexical_cast<uint64_t>(s.substr(pos + 1));
        }
        return std::make_pair(start, end);
    });
}

std::string S2CStatisticsStationResult::msg() const {
    return Message::msg("UI_STATISTICS_RESULT", "stationAlarm", {
        {"id", id}
    }, {});
}

C2SStatisticsType::C2SStatisticsType(const Message &msg) {
    id = msg.getString("id");
    startTime = boost::lexical_cast<uint64_t>(msg.getString("startTime"));
    endTime = boost::lexical_cast<uint64_t>(msg.getString("endTime"));
    // TODO(zf)
    // subcenterIds = msg.getArray("")
}

std::string S2CStatisticsTypeResult::msg() const {
    Message msg;
    msg.setType("UI_STATISTICS_RESULT");
    msg.setId("typeAlarm");
    msg.addString("id", id);
    std::unordered_map<std::string, std::vector<std::string>> arrays;
    for (auto &v : typeResult) {
        std::vector<std::string> val;
        val.push_back(std::to_string(std::get<0>(v.second)));
        val.push_back(std::to_string(std::get<1>(v.second)));
        val.push_back(std::to_string(std::get<2>(v.second)));
        arrays[v.first] = val;
    }
    msg.setArrays(arrays);
    return msg.toXmlWithLength();
}

C2SReport::C2SReport(const Message &msg) {
    id = msg.getString("id");
    startTime = boost::lexical_cast<uint64_t>(msg.getString("startTime"));
    endTime = boost::lexical_cast<uint64_t>(msg.getString("endTime"));
    stations = msg.getArray("stations");
}

std::string S2CReportResult::msg() const {
    Message msg;
    msg.setType("UI_REPORT_RESULT");
    msg.setId("alarm");
    msg.addString("id", id);
    std::unordered_map<std::string, std::vector<std::string>> arrays;
    for (auto &v : reportResult) {
        std::vector<std::string> val;
        val.push_back(std::to_string(std::get<0>(v.second)));
        val.push_back(std::to_string(std::get<1>(v.second)));
        val.push_back(std::to_string(std::get<2>(v.second)));
        val.push_back(std::to_string(std::get<3>(v.second)));
        arrays[v.first] = val;
    }
    msg.setArrays(arrays);
    return msg.toXmlWithLength();
}

C2SRawData::C2SRawData(const Message &msg) {
    id = msg.getString("id");
    subcenter = msg.getString("subcenter");
    station = msg.getString("station");
    device = msg.getString("device");
    startTime = boost::lexical_cast<uint64_t>(msg.getString("startTime"));
    endTime = boost::lexical_cast<uint64_t>(msg.getString("endTime"));
}

std::string S2CRawDataResult::msg() const {
    return Message::msg("UI_RAWDATA", "result", {
        {"id", id},
        {"messageType", messageType},
        {"messageID", messageID},
        {"timestamp", std::to_string(timestamp)}
    }, {});
}

std::string S2CLineBalise::msg() const {
    return Message::msg("UI_LINE_DISPLAY", "balise", {
        {"baliseID", baliseID},
        {"state", state}
    }, {});
}

std::string S2CT2GBalise::msg() const {
    std::string isRead_s;
    if (isRead)
        isRead_s = "true";
    else
        isRead_s = "false";

    return Message::msg("UI_TRAIN2GROUND_DISPLAY", "balise", {
        {"obcuID", obcuID},
        {"baliseID", baliseID},
        {"IsRead", isRead_s},
        {"reaction", reaction}
    }, {});
}

std::string S2CT2GTWC::msg() const {
    std::string isRead_s;
    if (isRead)
        isRead_s = "true";
    else
        isRead_s = "false";

    return Message::msg("UI_TRAIN2GROUND_DISPLAY", "balise", {
        {"obcuID", obcuID},
        {"TWCID", TWCID},
        {"IsRead", isRead_s},
        {"reaction", reaction}
    }, {});
}

std::string S2CSafetyLink::msg() const {
    std::string ifHaveData_s;
    if (ifHaveData == 0) {
        ifHaveData_s = "both_no_data";
    } else if (ifHaveData == 1) {
        ifHaveData_s = "single_data";
    } else {
        ifHaveData_s = "both_data";
    }
    std::string linkStatus_s;
    if (linkStatus == 0) {
        linkStatus_s = "false";
    } else if (linkStatus == 1) {
        linkStatus_s = "true";
    } else {
        linkStatus_s = "fake_true";
    }

    if (isLine) {
        return Message::msg("UI_LINE_DISPLAY", "safetylink", {
            {"IfHaveData", ifHaveData_s},
            {"Info", info},
            {"LinkStatus", linkStatus_s},
            {"ChannelId", channelId}
        }, {});
    } else {
        return Message::msg("UI_DEVICE_DISPLAY", "safetylink", {
            {"IfHaveData", ifHaveData_s},
            {"Info", info},
            {"LinkStatus", linkStatus_s},
            {"ChannelId", channelId}
        }, {});
    }
}

Message_Identifier S2CSafetyLink::getMessageIdentifier() const {
    if (isLine) {
        return S2C_LineSafetyLink;
    } else {
        return S2C_DeviceSafetyLink;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 配置信息
std::string S2CWarnConfigConfig::msg() const {
    return Message::msg("UI_WARNCONFIG", "config", {}, {});
}

C2SWarnConfigModify::C2SWarnConfigModify(const Message &msg) {
    devicetype = msg.getString("devicetype");
    type = msg.getString("type");
    code = msg.getString("code");
    level = msg.getString("level");
    measure = msg.getString("measure");
    description = msg.getString("description");
}

std::string S2CWarnConfigConfirm::msg() const {
    if (success) {
        return Message::msg("UI_WARNCONFIG", "confirm", {
            {"success", "true"}
        }, {});
    } else {
        return Message::msg("UI_WARNCONFIG", "confirm", {
            {"success", "false"},
            {"reason", reason}
        }, {});
    }
}

C2SEarlywarnConfigModify::C2SEarlywarnConfigModify(const Message &msg)
    : C2SWarnConfigModify(msg) {
}

std::string S2CEarlywarnConfigConfirm::msg() const {
    if (success) {
        return Message::msg("UI_EARLYWARNCONFIG", "confirm", {
            {"success", "true"}
        }, {});
    } else {
        return Message::msg("UI_EARLYWARNCONFIG", "confirm", {
            {"success", "false"},
            {"reason", reason}
        }, {});
    }
}

std::string S2CEventConfigConfig::msg() const {
    return Message::msg("UI_EVENTCONFIG", "config", {}, {});
}

C2SStaticConfigInitial::C2SStaticConfigInitial(const Message &msg) {
    stations = msg.getArray("station");
}

std::string S2CStaticConfigDevice::msg() const {
    return Message::msg("UI_STATIC_CONFIG", "device_config", {}, {});
}

std::string S2CStaticConfigTelegram::msg() const {
    return Message::msg("UI_STATIC_CONFIG", "telegram_contrast", {}, {});
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 综合状态展示
std::string S2CCIMStatus::msg() const {
    return Message::msg("UI_CIM", "status", {
        {"ci", ci},
        {"index", index},
        {"status", status},
        {"description", description}
    }, {});
}

std::string S2CCIMKxStatus::msg() const {
    return Message::msg("UI_CIM", "kx_status", {
        {"ci", ci},
        {"id", id},
        {"status", status},
        {"status1", status1},
        {"status2", status2},
        {"description", description}
    }, {});
}




Message_Identifier S2CCIMEt::getMessageIdentifier() const {
    if (id.find("In") != std::string::npos) {
        return S2C_CIM_EtIn;
    } else {
        return S2C_CIM_EtOut;
    }
}

std::string S2CCIMEt::msg() const {
    Message msg;
    msg.setType("UI_CIM");
    // in or out
    int pos = 0;
    try {
        if (id.find("In") != std::string::npos) {
            msg.setId("et_in");
            pos = std::stoi(id.substr(2));
        } else {
            msg.setId("et_out");
            pos = std::stoi(id.substr(3));
        }
    } catch (const std::exception &e) {
        log_error << e.what();
    }
    // ci
    msg.addString("ci", ci);
    // pos
    msg.addString("pos", std::to_string(pos));
    // status
    msg.addString("status", status);
    msg.addString("status1", status1);
    msg.addString("status2", status2);

    return msg.toXmlWithLength();
}

// S2C_CIM_EtConfig
std::string S2CCIMEtConfig::msg() const {
    std::string msgs;
    for (const auto &v : pioList) {
        std::vector<std::string> pio_id_v;
        std::vector<std::string> pio_type_v;

        auto &pioMap = v.second;
        std::transform(pioMap.begin(), pioMap.end(), std::back_inserter(pio_id_v),
            [](const std::pair<std::string, S2CCIMEt> &p) {
                return p.first;
            });
        std::transform(pioMap.begin(), pioMap.end(), std::back_inserter(pio_type_v),
            [](const std::pair<std::string, S2CCIMEt> &p) {
                return p.second.type;
            });

        msgs += Message::msg("UI_CIM", "et_config", {
            {"ci", v.first}
        }, {
            {"index", pio_id_v},
            {"name", pio_id_v},
            {"type", pio_type_v}
        });
    }
    return msgs;
}
