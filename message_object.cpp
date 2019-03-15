#include "message_object.h"

// std
#include <utility>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <fstream>

// boost
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>



// local
#include "message.h"

#define READS(str) pt.get<std::string>(str)
#define READ64(str) pt.get<int64_t>(str)
#define READU64(str) pt.get<uint64_t>(str)
#define PREADS(str) parse(READS(str))

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// only for internal use
std::string parse(const std::string &s);

std::shared_ptr<MessageObject> createMessageObject(const std::string &identifier) {
    #define REGISTE(objname, obj)                       \
    if (identifier == objname) {                        \
        return std::make_shared<obj>();                 \
    }

    REGISTE("S2C_OverallStatus_DeviceAlarmItem", S2COverallStatusDeviceAlarmItem)
    REGISTE("S2C_OverallStatus_DeviceEarlywarnItem", S2COverallStatusDeviceEarlywarnItem)
    REGISTE("S2C_OverallStatus_DeviceStatus", S2COverallStatusDeviceStatus)
    REGISTE("S2C_OverallStatus_StationAlarmItem", S2COverallStatusStationAlarmItem)
    REGISTE("S2C_OverallStatus_StationEarlywarnItem", S2COverallStatusStationEarlywarnItem)
    REGISTE("S2C_OverallStatus_DeviceAlarm", S2COverallStatusDeviceAlarm)
    REGISTE("S2C_OverallStatus_StationEarlywarn", S2COverallStatusStationEarlywarn)
    REGISTE("S2C_OverallStatus_StationAlarm", S2COverallStatusStationAlarm)



    return nullptr;
}

void saveRecord(const std::string &msg, const std::string &type, const std::string &id) {
    namespace fs = boost::filesystem;

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S.log");
    auto p = fs::path {"."} / "data" / "query" / "record" / type / id;
    if (!fs::exists(p)) {
        log_info << p << " is not exist, create ...";
        if (!fs::create_directories(p)) {
            log_error << "can not create path " << p;
            return;
        }
    } else if (!fs::is_directory(p)) {
        log_error << "save msg error: path " << p << "is not a directory.";
        return;
    }
    auto fp {p / ss.str()};
    fs::ofstream out {fp, std::ios::app};

    if (out.is_open()) {
        out << msg;
        out.close();
    }


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageObject> getMessageObject(const std::string &msg) {
    Message xmsg(msg);
    auto type = xmsg.getType();
    auto id = xmsg.getId();

    if (type == "UI_AUTHENTICATION") {
        return std::make_shared<C2SAuthentication>(xmsg);
    } else if (type == "UI_CONFIGURATION") {
        return std::make_shared<C2SConfiguration>(xmsg);
    } else if (type == "UI_SUBSCRIPTION" || type == "UI_UNSUBSCRIPTION") {
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
    } else {
        saveRecord(msg, type, id);
        return std::make_shared<C2SQuery>(type, id);
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

    id = msg.getId();
    ruleSet = msg.getString("ruleset");
    level = msg.getString("level");
    auto deviceId = msg.getString("id");
    if (deviceId != "")
        ids.push_back(deviceId);

    auto t_ids = msg.getArray("id");
    std::copy(t_ids.begin(), t_ids.end(), std::back_inserter(ids));
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

C2SQuery::C2SQuery(const std::string &type, const std::string &id)
    : type(type), id(id) {

}



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
void S2COverallStatusDeviceAlarmItem::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    id = READS("device");
    isNew = READS("new");
    AlarmItem alarm;
    for (const auto &item : pt.get_child("items")) {
        alarm.device = item.second.get<std::string>("device");
        alarm.id = item.second.get<std::string>("_id");
        alarm.time = item.second.get<uint64_t>("time");
        alarm.level = item.second.get<std::string>("level");
        alarm.type = item.second.get<std::string>("type");
        alarm.description = item.second.get<std::string>("description");
        alarm.isRecover = item.second.get<std::string>("recover");
        alarm.recoverTime = item.second.get<uint64_t>("recover_time");
        alarm.measure = item.second.get<std::string>("measure");
        alarm.isConfirm = item.second.get<std::string>("confirm");
        alarm.confirmTime = item.second.get<uint64_t>("confirm_time");
        alarm.confirmDesc = item.second.get<std::string>("confirm_dsc");
        itemList.push_back(alarm);
    }
}

std::string S2COverallStatusDeviceAlarmItem::msg() const {
    Message res;
    res.setType("UI_OVERALL_STATUS");
    res.setId("deviceAlarmItem");
    res.addString("station", station);
    res.addString("device", id);
    res.addString("new", isNew);
    std::vector<Message> msgs;
    for (const auto &item : itemList) {
        Message msg;
        msg.addString("device", item.device);
        msg.addString("_id", item.id);
        msg.addString("time", item.time);
        msg.addString("level", item.level);
        msg.addString("type", item.type);
        msg.addString("description", item.description);
        msg.addString("recover", item.isRecover);
        msg.addString("recover_time", item.recoverTime);
        msg.addString("measure", item.measure);
        msg.addString("confirm", item.isConfirm);
        msg.addString("confirm_time", item.confirmTime);
        msg.addString("confirm_dsc", item.confirmDesc);
        msgs.push_back(msg);
    }
    res.addPacketArray("items", msgs);
    return res.toXmlWithLength();
}

void S2COverallStatusDeviceEarlywarnItem::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    id = READS("device");
    isNew = READS("new");
    AlarmItem alarm;
    for (const auto &item : pt.get_child("items")) {
        alarm.device = item.second.get<std::string>("device");
        alarm.id = item.second.get<std::string>("_id");
        alarm.time = item.second.get<uint64_t>("time");
        alarm.level = item.second.get<std::string>("level");
        alarm.type = item.second.get<std::string>("type");
        alarm.description = item.second.get<std::string>("description");
        alarm.isRecover = item.second.get<std::string>("recover");
        alarm.recoverTime = item.second.get<uint64_t>("recover_time");
        alarm.measure = item.second.get<std::string>("measure");
        alarm.isConfirm = item.second.get<std::string>("confirm");
        alarm.confirmTime = item.second.get<uint64_t>("confirm_time");
        alarm.confirmDesc = item.second.get<std::string>("confirm_dsc");
        itemList.push_back(alarm);
    }
}

std::string S2COverallStatusDeviceEarlywarnItem::msg() const {
    Message res;
    res.setType("UI_OVERALL_STATUS");
    res.setId("deviceEarlywarnItem");
    res.addString("station", station);
    res.addString("device", id);
    res.addString("new", isNew);
    std::vector<Message> msgs;
    for (const auto &item : itemList) {
        Message msg;
        msg.addString("device", item.device);
        msg.addString("_id", item.id);
        msg.addString("time", item.time);
        msg.addString("level", item.level);
        msg.addString("type", item.type);
        msg.addString("description", item.description);
        msg.addString("recover", item.isRecover);
        msg.addString("recover_time", item.recoverTime);
        msg.addString("measure", item.measure);
        msg.addString("confirm", item.isConfirm);
        msg.addString("confirm_time", item.confirmTime);
        msg.addString("confirm_dsc", item.confirmDesc);
        msgs.push_back(msg);
    }
    res.addPacketArray("items", msgs);
    return res.toXmlWithLength();
}

void S2COverallStatusDeviceStatus::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    id = READS("device");
    status = PREADS("status");
    for (const auto &v : pt.get_child("description")) {
        description.push_back(v.first);
    }
}

std::string S2COverallStatusDeviceStatus::msg() const {
    Message res;
    res.setType("UI_OVERALL_STATUS");
    res.setId("deviceStatus");
    res.addString("station", station);
    res.addString("deviceId", id);
    res.addString("status", status);
    res.addArray("description", description);
    std::vector<Message> msgs;
    for (const auto &deviceStatus : deviceStatusList) {
        Message msg;
        msg.addString("linkId", deviceStatus.linkId);
        msg.addString("linkStatus", deviceStatus.linkStatus);
        msgs.push_back(msg);
    }
    res.addPacketArray(deviceType, msgs);
    return res.toXmlWithLength();
}

void S2COverallStatusStationAlarmItem::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    isNew = READS("new");
    AlarmItem alarm;
    for (const auto &item : pt.get_child("items")) {
        alarm.device = item.second.get<std::string>("device");
        alarm.id = item.second.get<std::string>("_id");
        alarm.time = item.second.get<uint64_t>("time");
        alarm.level = item.second.get<std::string>("level");
        alarm.type = item.second.get<std::string>("type");
        alarm.description = item.second.get<std::string>("description");
        alarm.isRecover = item.second.get<std::string>("recover");
        alarm.recoverTime = item.second.get<uint64_t>("recover_time");
        alarm.measure = item.second.get<std::string>("measure");
        alarm.isConfirm = item.second.get<std::string>("confirm");
        alarm.confirmTime = item.second.get<uint64_t>("confirm_time");
        alarm.confirmDesc = item.second.get<std::string>("confirm_dsc");
        itemList.push_back(alarm);
    }
}

std::string S2COverallStatusStationAlarmItem::msg() const {
    Message res;
    res.setType("UI_OVERALL_STATUS");
    res.setId("stationAlarmItem");
    res.addString("station", station);
    res.addString("new", isNew);
    std::vector<Message> msgs;
    for (const auto &item : itemList) {
        Message msg;
        msg.addString("device", item.device);
        msg.addString("_id", item.id);
        msg.addString("time", item.time);
        msg.addString("level", item.level);
        msg.addString("type", item.type);
        msg.addString("description", item.description);
        msg.addString("recover", item.isRecover);
        msg.addString("recover_time", item.recoverTime);
        msg.addString("measure", item.measure);
        msg.addString("confirm", item.isConfirm);
        msg.addString("confirm_time", item.confirmTime);
        msg.addString("confirm_dsc", item.confirmDesc);
        msgs.push_back(msg);
    }
    res.addPacketArray("items", msgs);
    return res.toXmlWithLength();
}

void S2COverallStatusStationEarlywarnItem::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    isNew = READS("new");
    AlarmItem alarm;
    for (const auto &item : pt.get_child("items")) {
        alarm.device = item.second.get<std::string>("device");
        alarm.id = item.second.get<std::string>("_id");
        alarm.time = item.second.get<uint64_t>("time");
        alarm.level = item.second.get<std::string>("level");
        alarm.type = item.second.get<std::string>("type");
        alarm.description = item.second.get<std::string>("description");
        alarm.isRecover = item.second.get<std::string>("recover");
        alarm.recoverTime = item.second.get<uint64_t>("recover_time");
        alarm.measure = item.second.get<std::string>("measure");
        alarm.isConfirm = item.second.get<std::string>("confirm");
        alarm.confirmTime = item.second.get<uint64_t>("confirm_time");
        alarm.confirmDesc = item.second.get<std::string>("confirm_dsc");
        itemList.push_back(alarm);
    }
}

std::string S2COverallStatusStationEarlywarnItem::msg() const {
    Message res;
    res.setType("UI_OVERALL_STATUS");
    res.setId("stationEarlywarnItem");
    res.addString("station", station);
    res.addString("new", isNew);
    std::vector<Message> msgs;
    for (const auto &item : itemList) {
        Message msg;
        msg.addString("device", item.device);
        msg.addString("_id", item.id);
        msg.addString("time", item.time);
        msg.addString("level", item.level);
        msg.addString("type", item.type);
        msg.addString("description", item.description);
        msg.addString("recover", item.isRecover);
        msg.addString("recover_time", item.recoverTime);
        msg.addString("measure", item.measure);
        msg.addString("confirm", item.isConfirm);
        msg.addString("confirm_time", item.confirmTime);
        msg.addString("confirm_dsc", item.confirmDesc);
        msgs.push_back(msg);
    }
    res.addPacketArray("items", msgs);
    return res.toXmlWithLength();
}

void S2COverallStatusDeviceAlarm::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    id = READS("deviceID");
    level1 = READ64("level1");
    level2 = READ64("level2");
    level3 = READ64("level3");
    for (const auto &v : pt.get_child("description")) {
        description.push_back(v.first);
    }
}

std::string S2COverallStatusDeviceAlarm::msg() const {
    return Message::msg("UI_OVERALL_STATUS", "deviceAlarm", {
        {"station", station},
        {"device", id},
        {"level1", std::to_string(level1)},
        {"level2", std::to_string(level2)},
        {"level3", std::to_string(level3)},
    }, {
        {"description", description}
    });
}

void S2COverallStatusDeviceEarlywarn::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    id = READS("deviceID");
    number = READ64("number");
    for (const auto &v : pt.get_child("description")) {
        description.push_back(v.first);
    }
}

std::string S2COverallStatusDeviceEarlywarn::msg() const {
    return Message::msg("UI_OVERALL_STATUS", "deviceEarlywarn", {
        {"station", station},
        {"device", id},
        {"number", std::to_string(number)},
    }, {
        {"description", description}
    });
}

void S2COverallStatusStationEarlywarn::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    number = READ64("number");
    for (const auto &v : pt.get_child("description")) {
        description.push_back(v.first);
    }
}

std::string S2COverallStatusStationEarlywarn::msg() const {
    return Message::msg("UI_OVERALL_STATUS", "deviceEarlywarn", {
        {"station", station},
        {"number", std::to_string(number)},
    }, {
        {"description", description}
    });
}

void S2COverallStatusStationAlarm::retrieveData(const boost::property_tree::ptree &pt) {
    station = READS("station");
    level1 = READ64("level1");
    level2 = READ64("level2");
    level3 = READ64("level3");
    for (const auto &v : pt.get_child("description")) {
        description.push_back(v.first);
    }
}

std::string S2COverallStatusStationAlarm::msg() const {
    return Message::msg("UI_OVERALL_STATUS", "deviceAlarm", {
        {"station", station},
        {"level1", std::to_string(level1)},
        {"level2", std::to_string(level2)},
        {"level3", std::to_string(level3)},
    }, {
        {"description", description}
    });
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 联锁状态展示
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


std::string parse(const std::string &s) {
    // if has "|"
    uint32_t lastPos{0}, splitPos;
    std::vector<std::string> choices;

    while ((splitPos = s.find("|", lastPos)) != std::string::npos) {
        if (splitPos != 0 && s.at(splitPos - 1) == '\\') continue;
        choices.push_back(s.substr(lastPos, splitPos - lastPos));
        lastPos = splitPos + 1;
    }
    choices.push_back(s.substr(lastPos));

    auto res = choices.at(rand() % choices.size());
    // rand_r

    return res;
}