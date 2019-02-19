#ifndef MESSAGE_OBJECT_H_
#define MESSAGE_OBJECT_H_

// std
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <utility>
#include <tuple>

// local
#include "global.h"
#include "message.h"


// identifiers
enum Message_Identifier {
// 控制协议
    // 认证
    C2S_Authentication,
    S2C_AuthenticationReply,
    C2S_Configuration,
    S2C_ConfigurationReply,
    // 订阅
    C2S_Subscription,
    // C2S_Unsubscription,
    // 回放
    C2S_Playback_Start,
    C2S_Playback_Option,
    C2S_Playback_Control,
    S2C_Playback_Time,
    // 心跳
    C2S_HeartBeat,
    S2C_HeartBeat,
    // 关闭
    C2S_Close,

// 查询
    C2S_Query_Select,
    C2S_Query_Count,
    C2S_Query_Distinct,
    C2S_Query_Average,
    C2S_Query_Max,
    C2S_Query_Min,
    S2C_Query_SelectResult,
    S2C_Query_CountResult,
    S2C_Query_DistinctResult,
    S2C_Query_AverageResult,
    S2C_Query_MaxResult,
    S2C_Query_MinResult,
    C2S_Statistics_Device,
    S2C_Statistics_DeviceResult,
    C2S_Statistics_Station,
    S2C_Statistics_StationResult,
    C2S_Statistics_Type,
    S2C_Statistics_TypeResult,
    C2S_Report,
    S2C_ReportResult,
    C2S_RawData,
    S2C_RawDataResult,
    S2C_LineLEU,
    S2C_DeviceLEU,
    S2C_LineBalise,
    S2C_T2GBalise,
    S2C_T2GTWC,
    S2C_LineSafetyLink,
    S2C_DeviceSafetyLink,
// 配置信息
    C2S_WarnConfig_Initial,
    S2C_WarnConfig_Config,
    C2S_WarnConfig_Modify,
    S2C_WarnConfig_Confirm,
    C2S_EarlywarnConfig_Initial,
    S2C_EarlywarnConfig_Config,
    C2S_EarlywarnConfig_Modify,
    S2C_EarlywarnConfig_Confirm,
    C2S_EventConfig_Initial,
    S2C_EventConfig_Config,
    C2S_StaticConfig_Initial,
    S2C_StaticConfig_Device,
    S2C_StaticConfig_Telegram,
// 线路级展示
    S2C_Line_TrackStatus,
    S2C_Line_Switch,
    S2C_Line_Signal,
    S2C_Line_TrackCode,
    S2C_Line_TrackLock,
    S2C_Line_C3MA,
    S2C_Line_C3TPR,
    S2C_Line_BaliseTelContrast,
    S2C_Line_BaliseTel,
    S2C_Line_BaliseConfig,
    S2C_Line_CTCSBaliseStatistics,
    S2C_Line_CTCSBalise,
    S2C_Line_CTCSTrackStatusStatistics,
    S2C_Line_CTCSTrackStatus,
    C2S_Line_EarlywarnConfirm,
    S2C_Line_CTCSSwitchStatistics,
    S2C_Line_CTCSSwitch,
    S2C_Line_CTCSTrackConsistentStatistics,
    S2C_Line_CTCSTrackConsistent,
    S2C_Line_CTCSRadioStatistics,
    S2C_Line_CTCSRadio,
    S2C_Line_CTCSButtonConfig,
    S2C_Line_CTCSButtonStatistics,
    S2C_Line_TrackOccupyStatistics,
    S2C_Line_CTCSDeviceConfig,
    S2C_Line_OpenTrack,
    S2C_Line_TrackOverallStatus,
    C2S_Consult_Switch,
    C2S_Consult_OpenTrack,
    C2S_Consult_Occupy,
    C2S_Consult_Track,
    C2S_Consult_Balise,
    C2S_Consult_Radio,
    S2C_Line_NeutralZone,
    S2C_Line_TrackConfig,
    C2S_Consult_DCTime,
    C2S_Consult_DCCurve_P,
    C2S_Consult_DCCurve_A,
    S2C_Line_DCCurveTime,
    S2C_Line_DCCurveValue,
    S2C_Line_DC_ID,
    S2C_Line_SignalDegrade,
    S2C_Line_SWD_dsr,
    S2C_Line_SWD_initial,
    S2C_Line_SWD_info,
    S2C_Line_SWD_status,
    S2C_Line_SWD_rawdata,
    S2C_Line_CTCSDegradeConsistentStatistics,
    S2C_Line_CTCSDegradeConsistent,
    S2C_Line_Degrade_overall_status,
    C2S_Consult_Degrade,
    S2C_Line_CTCSDisasterConsistentStatistics,
    S2C_Line_CTCSDisasterConsistent,
    S2C_Line_Disaster_overall_status,
    C2S_Consult_Disaster,
    S2C_Line_CTCSTrackCodeConsistentStatistics,
    S2C_Line_CTCSTrackCodeConsistent,
    S2C_Line_TrackCode_overall_status,
    C2S_Consult_TrackCode,
    S2C_Line_SignalStatus,
    S2C_Line_SignalChangeStatus,
    S2C_Line_SignalStatusCheck,
    S2C_Line_SignalChangeStatusCheck,
    S2C_TCCM_IntervalDir,
    S2C_TCCM_BorderBlock,
    S2C_TCCM_AlarmStatus,
// 综合状态展示
    S2C_OverallStatus_DeviceStatus,
    S2C_OverallStatus_StationStatus,
    S2C_OverallStatus_StatusSummary,
    S2C_OverallStatus_LinkSummary,
    S2C_OverallStatus_DeviceAlarmItem,
    S2C_OverallStatus_StationAlarmItem,
    S2C_OverallStatus_DeviceAlarm,
    S2C_OverallStatus_StationAlarm,
    S2C_OverallStatus_AlarmSummary,
    S2C_OverallStatus_DeviceEarlywarnItem,
    S2C_OverallStatus_StationEarlywarnItem,
    S2C_OverallStatus_DeviceEarlywarn,
    S2C_OverallStatus_StationEarlywarn,
    S2C_OverallStatus_EarlywarnSummary,
// 联锁状态展示
    S2C_CIM_Status,
    S2C_CIM_KxStatus,
    S2C_CIM_LinkStatus,
    S2C_CIM_SwitchLock,
    S2C_CIM_SwitchClose,
    S2C_CIM_Lock,
    S2C_CIM_SwitchStatus,
    S2C_CIM_SwitchCmd,
    S2C_CIM_SignalStatus,
    S2C_CIM_SignalCmd,
    S2C_CIM_TrackStatus,
    S2C_CIM_EtIn,
    S2C_CIM_EtOut,
    S2C_CIM_Route,
    S2C_CIM_Button,
    S2C_CIM_Special,
    S2C_CIM_WarnConfirm,
    S2C_CIM_Operation,
    S2C_CIM_OpenLine,
    S2C_CIM_EtConfig,
    S2C_CIM_Degrade,

    Invalid_Identifier
};


struct MessageObject {
    virtual Message_Identifier getMessageIdentifier() const = 0;
    virtual std::string msg() const {
        return "";
    }
};

#define DECLARE_IDENTIFIER()                                \
Message_Identifier getMessageIdentifier() const override;


#define IDENTIFIER(identifier)                       \
Message_Identifier getMessageIdentifier() const override {  \
    return identifier;                                      \
}

#define DECLARE_MSG()                                       \
std::string msg() const override;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<MessageObject> getMessageObject(const std::string &msg);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 控制协议
struct C2SAuthentication : public MessageObject {
    std::string username;
    std::string password;

    C2SAuthentication() = default;
    explicit C2SAuthentication(const Message &msg);

    IDENTIFIER(C2S_Authentication)
};

struct S2CAuthenticationReply : public MessageObject {
    bool success{false};
    std::string reason;

    DECLARE_MSG()
    IDENTIFIER(C2S_Authentication)
};

struct C2SConfiguration : public MessageObject {
    std::string softwareVersion;
    std::string dataVersion;

    C2SConfiguration() = default;
    explicit C2SConfiguration(const Message &msg);

    IDENTIFIER(C2S_Configuration)
};

struct S2CConfigurationReply : public MessageObject {
    bool success{false};

    DECLARE_MSG()
    IDENTIFIER(S2C_ConfigurationReply)
};

struct C2SSubscription : public MessageObject {
    bool subscription{true};
    Message_Type type;
    RuleSet ruleSet;
    Subscribe_Level level;
    std::string id;

    C2SSubscription() = default;
    explicit C2SSubscription(const Message &msg);

    IDENTIFIER(C2S_Subscription)
};

// playback
struct C2SPlayback_Start : public MessageObject{
    uint64_t startTime;
    uint64_t endTime;
    float rate;

    C2SPlayback_Start() = default;
    explicit C2SPlayback_Start(const Message &msg);

    IDENTIFIER(C2S_Playback_Start)
};

struct C2SPlayback_Option : public MessageObject {
    float rate;

    C2SPlayback_Option() = default;
    explicit C2SPlayback_Option(const Message &msg);

    IDENTIFIER(C2S_Playback_Option)
};

struct C2SPlayback_Control : public MessageObject {
    Playback_Control control;

    C2SPlayback_Control() = default;
    explicit C2SPlayback_Control(const Message &msg);

    IDENTIFIER(C2S_Playback_Control);
};

struct S2CPlayback_Time : public MessageObject {
    uint64_t curTime;

    DECLARE_MSG()
    IDENTIFIER(S2C_Playback_Time);
};

// heartbeat
struct C2SHeartBeat : public MessageObject {
    IDENTIFIER(C2S_HeartBeat)
};

struct S2CHeartBeat : public MessageObject {
    DECLARE_MSG()
    IDENTIFIER(S2C_HeartBeat)
};

// close
struct C2SClose : public MessageObject {
    IDENTIFIER(C2S_Close)
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 查询
struct C2SQueryBase : public MessageObject {
    std::string id;
    std::string db;
    uint64_t startTime;
    uint64_t endTime;

    explicit C2SQueryBase(const Message &msg);
    IDENTIFIER(Invalid_Identifier)
};


struct C2SQuerySelect : public C2SQueryBase {
    explicit C2SQuerySelect(const Message &msg);
    IDENTIFIER(C2S_Query_Select)
};

struct C2SQueryCount : public C2SQueryBase {
    explicit C2SQueryCount(const Message &msg);
    IDENTIFIER(C2S_Query_Count)
};

struct C2SQueryDistinct : public C2SQueryBase {
    std::string key;

    explicit C2SQueryDistinct(const Message &msg);
    IDENTIFIER(C2S_Query_Distinct)
};

struct C2SQueryAverage : public C2SQueryDistinct {
    explicit C2SQueryAverage(const Message &msg);
    IDENTIFIER(C2S_Query_Average)
};

struct C2SQueryMax : public C2SQueryDistinct {
    explicit C2SQueryMax(const Message &msg);
    IDENTIFIER(C2S_Query_Max)
};

struct C2SQueryMin : public C2SQueryDistinct {
    explicit C2SQueryMin(const Message &msg);
    IDENTIFIER(C2S_Query_Min)
};

struct S2CQuerySelectResult : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Query_SelectResult)
};

struct S2CQueryCountResult : public MessageObject {
    std::string id;
    uint64_t result;

    DECLARE_MSG()
    IDENTIFIER(S2C_Query_CountResult)
};

struct S2CQueryDistinctResult : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Query_DistinctResult)
};

struct S2CQueryAverageResult : public MessageObject {
    std::string id;
    uint64_t result;

    DECLARE_MSG()
    IDENTIFIER(S2C_Query_AverageResult)
};

struct S2CQueryMaxResult : public MessageObject {
    std::string id;
    uint64_t result;

    DECLARE_MSG()
    IDENTIFIER(S2C_Query_MaxResult)
};

struct S2CQueryMinResult : public MessageObject {
    std::string id;
    uint64_t result;

    DECLARE_MSG()
    IDENTIFIER(S2C_Query_MinResult)
};

struct C2SStatisticsDevice : public MessageObject {
    std::string id;
    std::vector<std::pair<uint64_t, uint64_t>> timeUnits;
    // TODO(zf)

    explicit C2SStatisticsDevice(const Message &msg);
    IDENTIFIER(C2S_Statistics_Device)
};

struct S2CStatisticsDeviceResult : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Statistics_DeviceResult)
};

struct C2SStatisticsStation : public MessageObject {
    std::string id;
    // TODO(zf)
    std::vector<std::pair<uint64_t, uint64_t>> timeUnits;

    explicit C2SStatisticsStation(const Message &msg);
    IDENTIFIER(C2S_Statistics_Station)
};

struct S2CStatisticsStationResult : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Statistics_StationResult)
};

struct C2SStatisticsType : public MessageObject {
    std::string id;
    uint64_t startTime;
    uint64_t endTime;
    std::vector<std::string> subcenterIds;
    std::vector<std::string> types;

    explicit C2SStatisticsType(const Message &msg);
    IDENTIFIER(C2S_Statistics_Type)
};

struct S2CStatisticsTypeResult : public MessageObject {
    std::string id;
    std::map<std::string, std::tuple<uint16_t, uint16_t, uint16_t>> typeResult;

    DECLARE_MSG()
    IDENTIFIER(S2C_Statistics_TypeResult)
};

struct C2SReport : public MessageObject {
    std::string id;
    uint64_t startTime;
    uint64_t endTime;
    std::vector<std::string> stations;

    explicit C2SReport(const Message &msg);
    IDENTIFIER(C2S_Report)
};

struct S2CReportResult : public MessageObject {
    std::string id;
    std::map<std::string, std::tuple<uint16_t, uint16_t, uint16_t, uint16_t>> reportResult;

    DECLARE_MSG()
    IDENTIFIER(S2C_ReportResult)
};

struct C2SRawData : public MessageObject {
    std::string id;
    std::string subcenter;
    std::string station;
    std::string device;
    uint64_t startTime;
    uint64_t endTime;
    // TODO(zf)

    explicit C2SRawData(const Message &msg);
    IDENTIFIER(C2S_RawData)
};

struct S2CRawDataResult : public MessageObject {
    std::string id;
    std::string messageType;
    std::string messageID;
    uint64_t timestamp;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_RawDataResult)
};

// leu is not implemented

struct S2CLineBalise : public MessageObject {
    std::string baliseID;
    std::string state;

    DECLARE_MSG()
    IDENTIFIER(S2C_LineBalise)
};

struct S2CT2GBalise : public MessageObject {
    std::string obcuID;
    std::string baliseID;
    bool isRead;
    std::string reaction;

    DECLARE_MSG()
    IDENTIFIER(S2C_T2GBalise)
};

struct S2CT2GTWC : public MessageObject {
    std::string obcuID;
    std::string TWCID;
    bool isRead;
    std::string reaction;

    DECLARE_MSG()
    IDENTIFIER(S2C_T2GTWC)
};

struct S2CSafetyLink : public MessageObject {
    bool isLine;
    uint8_t ifHaveData;
    std::string info;
    uint8_t linkStatus;
    std::string channelId;

    DECLARE_MSG()
    DECLARE_IDENTIFIER()
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 配置信息
struct C2SWarnConfigInitial : public MessageObject {
    IDENTIFIER(C2S_WarnConfig_Initial)
};

struct S2CWarnConfigConfig : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_WarnConfig_Config)
};

struct C2SWarnConfigModify : public MessageObject {
    std::string devicetype;
    std::string type;
    std::string code;
    std::string level;
    std::string measure;
    std::string description;

    explicit C2SWarnConfigModify(const Message &msg);
    IDENTIFIER(C2S_WarnConfig_Modify)
};

struct S2CWarnConfigConfirm : public MessageObject {
    bool success{false};
    std::string reason;

    DECLARE_MSG()
    IDENTIFIER(S2C_WarnConfig_Confirm)
};

struct C2SEarlywarnConfigInitial : public MessageObject {
    IDENTIFIER(C2S_EarlywarnConfig_Initial)
};

struct S2CEarlywarnConfigConfig : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_EarlywarnConfig_Config)
};

struct C2SEarlywarnConfigModify : public C2SWarnConfigModify {
    explicit C2SEarlywarnConfigModify(const Message &msg);
    IDENTIFIER(C2S_EarlywarnConfig_Modify)
};

struct S2CEarlywarnConfigConfirm : public S2CWarnConfigConfirm {
    DECLARE_MSG()
    IDENTIFIER(S2C_EarlywarnConfig_Confirm)
};

struct C2SEventConfigInitial : public MessageObject {
    IDENTIFIER(C2S_EventConfig_Initial)
};

struct S2CEventConfigConfig : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_EventConfig_Config)
};

struct C2SStaticConfigInitial : public MessageObject {
    std::vector<std::string> stations;

    explicit C2SStaticConfigInitial(const Message &msg);
    IDENTIFIER(C2S_StaticConfig_Initial)
};

struct S2CStaticConfigDevice : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_StaticConfig_Device)
};

struct S2CStaticConfigTelegram : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_StaticConfig_Telegram)
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 线路级展示
struct S2CLineTrackStatus : public MessageObject {
    std::string id;
    uint8_t status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackStatus)
};

struct S2CLineSwitch : public MessageObject {
    std::string id;
    uint8_t status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_Switch)
};

struct S2CLineSignal : public MessageObject {
    std::string id;
    uint8_t status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_Signal)
};

struct S2CLineTrackCode : public MessageObject {
    std::string id;
    std::string code;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackCode)
};

struct S2CLineTrackLock : public MessageObject {
    std::string id;
    uint8_t status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackLock)
};

struct S2CLineC3MA : public MessageObject {
    bool direction;
    std::string RBC;
    std::string ATP;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_C3MA)
};

struct S2CLineC3TPR : public MessageObject {
    bool direction;
    std::string ATP;
    std::string trainNo;
    std::string mode;
    std::string level;
    std::string unit;
    bool switchdir;
    uint8_t location;
    bool switchstatus;
    uint8_t percent;
    uint16_t speed;
    std::string description;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_C3TPR)
};

struct S2CLineBaliseTelContrast : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_BaliseTelContrast)
};

struct S2CLineBaliseTel : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_BaliseTel)
};

struct S2CLineBaliseConfig : public MessageObject {
    std::string id;
    std::string name;
    std::string use;
    std::string lrbg;
    std::string milege;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_BaliseConfig)
};

struct S2CLineCTCSBaliseStatistics : public MessageObject {
    std::string id;
    uint16_t inconsistentNo;
    uint16_t baliseDefaultNo;
    uint16_t LEUDefaultNo;
    uint16_t TCCDefaultNo;
    uint16_t normalNo;
    uint16_t linkErrNo;
    uint16_t msgErrNo;
    uint16_t unlinkNo;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSBaliseStatistics)
};

struct S2CLineCTCSBalise : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSBalise)
};

struct S2CLineCTCSTrackStatusStatistics : public MessageObject {
    std::string id;
    uint16_t badOccNo;
    uint16_t badFreeNo;
    uint16_t flashNo;
    std::string type;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSTrackStatusStatistics)
};

struct S2CLineCTCSTrackStatus : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSTrackStatus)
};

struct C2SLineEarlywarnConfirm : public MessageObject {
    std::string id;
    // TODO(zf)

    explicit C2SLineEarlywarnConfirm(const Message &msg);
    IDENTIFIER(C2S_Line_EarlywarnConfirm)
};

struct S2CLineCTCSSwitchStatistics : public MessageObject {
    std::string id;
    uint16_t operateNo;
    uint16_t lossNo;
    uint16_t splitNo;
    uint16_t unknownNo;
    // TODO(zf)
    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSSwitchStatistics)
};

struct S2CLineCTCSSwitch : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSSwitch)
};

struct S2CLineCTCSTrackConsistentStatistics : public MessageObject {
    std::string id;
    uint16_t faultNo;
    std::string type;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSTrackConsistentStatistics)
};

struct S2CLineCTCSTrackConsistent : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSTrackConsistent)
};

struct S2CLineCTCSRadioStatistics : public MessageObject {
    std::string id;
    uint16_t faultNo;
    uint16_t msgErrNo;
    uint16_t linkErrNo;
    uint16_t seqErrNo;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSRadioStatistics)
};

struct S2CLineCTCSRadio : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSRadio)
};

struct S2CLineCTCSButtonConfig : public MessageObject {
    std::string ci;
    std::string name;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSButtonConfig)
};

struct S2CLineCTCSButtonStatistics : public MessageObject {
    std::string id;
    std::string index;
    std::string ci;
    std::string type;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSButtonStatistics)
};

struct S2CLineTrackOccupyStatistics : public MessageObject {
    std::string id;
    uint16_t occupyNo;
    uint64_t occupyTime;
    std::string type;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackOccupyStatistics)
};

struct S2CLineCTCSDeviceConfig : public MessageObject {
    std::string id;
    std::string station;
    std::string type;
    std::vector<std::string> tracks;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSDeviceConfig)
};

struct S2CLineOpenTrack : public MessageObject {
    std::string id;
    std::string type;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_OpenTrack)
};

struct S2CLineTrackOverallStatus : public MessageObject {
    std::string id;
    std::string type;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackOverallStatus)
};

struct C2SConsultBase : public MessageObject {
    std::string ruleset;
    std::string id;

    explicit C2SConsultBase(const Message &msg);
    IDENTIFIER(Invalid_Identifier)
};


struct C2SConsultSwitch : public MessageObject {
    explicit C2SConsultSwitch(const Message &msg);
    IDENTIFIER(C2S_Consult_Switch)
};

struct C2SConsultOpenTrack : public MessageObject {
    explicit C2SConsultOpenTrack(const Message &msg);
    IDENTIFIER(C2S_Consult_OpenTrack)
};

struct C2SConsultOccupy : public MessageObject {
    explicit C2SConsultOccupy(const Message &msg);
    IDENTIFIER(C2S_Consult_Occupy)
};

struct C2SConsultTrack : public MessageObject {
    explicit C2SConsultTrack(const Message &msg);
    IDENTIFIER(C2S_Consult_Track)
};

struct C2SConsultBalise : public MessageObject {
    explicit C2SConsultBalise(const Message &msg);
    IDENTIFIER(C2S_Consult_Balise)
};

struct C2SConsultRadio : public MessageObject {
    explicit C2SConsultRadio(const Message &msg);
    IDENTIFIER(C2S_Consult_Radio)
};

struct S2CLineNeutralZone : public MessageObject {
    std::string id;
    std::string start_mileage;
    std::string end_mileage;
    std::string name;
    uint32_t length;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_NeutralZone)
};

struct S2CLineTrackConfig : public MessageObject {
    std::string id;
    std::string mileage;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackConfig)
};

struct C2SConsultDCRelative : public MessageObject {
    std::string ruleset;
    std::string CSM;
    std::string subType;
    std::string ID;
    uint64_t unixTime;

    explicit C2SConsultDCRelative(const Message &msg);
    IDENTIFIER(Invalid_Identifier)
};

struct C2SConsultDCTime : public MessageObject {
    explicit C2SConsultDCTime(const Message &msg);
    IDENTIFIER(C2S_Consult_DCTime)
};

struct C2SConsultDCCurve_P : public MessageObject {
    explicit C2SConsultDCCurve_P(const Message &msg);
    IDENTIFIER(C2S_Consult_DCCurve_P)
};

struct C2SConsultDCCurve_A : public MessageObject {
    explicit C2SConsultDCCurve_A(const Message &msg);
    IDENTIFIER(C2S_Consult_DCCurve_A)
};

struct S2CLineDCCurveTime : public MessageObject {
    std::string CSM;
    std::string subType;
    std::vector<uint64_t> unixTimeList;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_DCCurveTime)
};

struct S2CLineDCCurveValue : public MessageObject {
    std::string CSM;
    std::string subType;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_DCCurveValue)
};

struct S2CLineDCID : public MessageObject {
    std::string CSM;
    std::vector<std::string> idList;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_DC_ID)
};

struct S2CLineSignalDegrade : public MessageObject {
    std::string id;
    uint8_t status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SignalDegrade)
};

struct S2CLineSWDDsr : public MessageObject {
    std::string id;
    std::string description;
    uint8_t status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SWD_dsr)
};

struct S2CLineSWDInitial : public MessageObject {
    std::string id;
    std::string mainNo;
    std::vector<std::string> showNo;
    std::vector<std::string> showID;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SWD_initial)
};

struct S2CLineSWDInfo : public MessageObject {
    std::string id;
    uint64_t startTime;
    uint64_t endTime;
    std::string NO;
    bool status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SWD_info)
};

struct S2CLineSWDStatus : public MessageObject {
    std::string id;
    std::string No;
    std::string oid;
    std::string description;
    bool status;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SWD_status)
};

struct S2CLineSWDRawData : public MessageObject {
    std::string id;
    std::string No;
    std::string oid;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SWD_rawdata)
};

struct S2CLineCTCSDegradeConsistentStatistics : public MessageObject {
    std::string id;
    uint32_t faultNo;
    std::vector<uint32_t> faultNo_month;
    std::vector<std::string> month;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSDegradeConsistentStatistics)
};

struct S2CLineCTCSDegradeConsistent : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSDegradeConsistent)
};

struct S2CLineDisasterOverallStatus : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_Disaster_overall_status)
};

struct C2SConsultDisaster : public MessageObject {
    std::string ruleset;
    std::string id;

    explicit C2SConsultDisaster(const Message &msg);
    IDENTIFIER(C2S_Consult_Disaster)
};

struct S2CLineCTCSTrackCodeConsistentStatistics : public MessageObject {
    std::string id;
    uint32_t faultNo;
    std::vector<uint32_t> faultNo_month;
    std::vector<std::string> month;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSTrackCodeConsistentStatistics)
};

struct S2CLineCTCSTrackCodeConsistent : public MessageObject {
    std::string id;
    bool fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_CTCSTrackCodeConsistent)
};

struct S2CLineCTCSTrackCodeOverallStatus : public MessageObject {
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_TrackCode_overall_status)
};

struct C2SConsultTrackCode : public MessageObject {
    std::string ruleset;
    std::string id;

    DECLARE_MSG()
    IDENTIFIER(C2S_Consult_TrackCode)
};

struct S2CLineSignalStatus : public MessageObject {
    std::string tcc;
    std::string ci;
    std::string description;
    uint64_t time;
    std::string id;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SignalStatus)
};

struct S2CLineSignalChangeStatus : public S2CLineSignalStatus {
    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SignalChangeStatus)
};

struct S2CLineSignalStatusCheck : public MessageObject {
    std::string status;
    std::string description;
    uint64_t time;
    std::string id;

    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SignalStatusCheck)
};

struct S2CLineSignalChangeStatusCheck : public S2CLineSignalStatusCheck {
    DECLARE_MSG()
    IDENTIFIER(S2C_Line_SignalChangeStatusCheck)
};

// tccm not implement



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 综合状态展示
struct S2COverallStatusDeviceStatus : public MessageObject {
    std::string station;
    std::string deviceId;
    uint8_t status;
    std::vector<std::string> description;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_DeviceStatus)
};

struct S2COverallStatusStationStatus : public MessageObject {
    std::string station;
    uint8_t status;
    uint8_t linkStatus;
    std::vector<std::string> description;

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_StationStatus)
};

struct S2COverallStatusStatusSummary : public MessageObject {
    std::vector<std::string> normal;
    std::vector<std::string> abnormal;
    std::vector<std::string> fault;

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_StatusSummary)
};

struct S2COverallStatusLinkSummary : public S2COverallStatusStatusSummary {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_LinkSummary)
};

struct S2COverallStatusItem : public MessageObject {
    std::string station;
    std::string device;
    bool isNew;
    // TODO(zf)
    IDENTIFIER(Invalid_Identifier)
};

struct S2COverallStatusDeviceAlarmItem : public MessageObject {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_DeviceAlarmItem)
};

struct S2COverallStatusStationAlarmItem : public MessageObject {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_StationAlarmItem)
};

struct S2COverallStatusDeviceAlarm : public MessageObject {
    std::string station;
    std::string device;
    uint32_t level1;
    uint32_t level2;
    uint32_t level3;
    std::vector<std::string> description;

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_DeviceAlarm)
};

struct S2COverallStatusStationAlarm : public S2COverallStatusDeviceAlarm {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_StationAlarm)
};

struct S2COverallStatusAlarmSummary : public MessageObject {
    uint32_t level1;
    uint32_t level2;
    uint32_t level3;
    std::vector<std::string> level1_stations;
    std::vector<std::string> level2_stations;

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_AlarmSummary)
};

struct S2COverallStatusDeviceEarlywarnItem : public S2COverallStatusItem {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_DeviceEarlywarnItem)
};

struct S2COverallStatusStationEarlywarnItem : public S2COverallStatusItem {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_StationEarlywarnItem)
};

struct S2COverallStatusDeviceEarlywarn : public MessageObject {
    std::string station;
    std::string device;
    uint32_t number;
    std::vector<std::string> description;

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_DeviceEarlywarn)
};

struct S2COverallStatusStationEarlywarn : public S2COverallStatusDeviceEarlywarn {
    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_StationEarlywarn)
};

struct S2COverallStatusEarlywarnSummary : public MessageObject {
    uint32_t number;
    std::vector<std::string> stations;

    DECLARE_MSG()
    IDENTIFIER(S2C_OverallStatus_EarlywarnSummary)
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 联锁状态展示
struct S2CCIMStatus : public MessageObject {
    std::string ci;
    std::string index;
    std::string status;
    std::string description;

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Status)
};

struct S2CCIMStatusBase : public MessageObject {
    std::string ci;
    std::string id;
    std::string status;
    std::string status1;
    std::string status2;
    std::string description;

    IDENTIFIER(Invalid_Identifier)
};

struct S2CCIMKxStatus : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_KxStatus)
};

struct S2CCIMLinkStatus : public S2CCIMStatusBase {
    uint16_t index;
    std::string text;

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_LinkStatus)
};

struct S2CCIMSwitchLock : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_SwitchLock)
};

struct S2CCIMSwitchClose : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_SwitchClose)
};

struct S2CCIMLock : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Lock)
};

struct S2CCIMSwitchStatus : public S2CCIMStatusBase {
    std::string relay1;
    std::string relay2;

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_SwitchStatus)
};

struct S2CCIMSwitchCmd : public S2CCIMSwitchStatus {
    std::string cmd;
    std::string cmd1;
    std::string cmd2;

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_SwitchCmd)
};

struct S2CCIMSignalStatus : public S2CCIMSwitchStatus {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_SignalStatus)
};

struct S2CCIMSignalCmd : public S2CCIMSignalStatus {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_SignalCmd)
};

struct S2CCIMTrackStatus : public S2CCIMSwitchStatus {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_TrackStatus)
};

struct S2CCIMEt : public MessageObject {
    std::string ci;
    std::string id;
    std::string type;
    std::string status;
    std::string status1;
    std::string status2;
    std::string description;

    DECLARE_MSG()
    DECLARE_IDENTIFIER()
};

struct S2CCIMRoute : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Route)
};

struct S2CCIMButton : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Button)
};

struct S2CCIMSpecial : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Special)
};

struct S2CCIMWarnConfirm : public MessageObject {
    std::string id;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_WarnConfirm)
};

struct S2CCIMOperation : public MessageObject {
    std::string ci;
    // TODO(zf)

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Operation)
};

struct S2CCIMOpenLine : public MessageObject {
    std::string ci;
    std::string id;
    uint8_t dir;
    uint8_t dir1;
    uint8_t dir2;
    uint8_t occ;
    uint8_t occ1;
    uint8_t occ2;
    uint8_t dj;
    uint8_t dj1;
    uint8_t dj2;

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_OpenLine)
};

struct S2CCIMEtConfig : public MessageObject {
    std::map<std::string, std::map<std::string, S2CCIMEt>> pioList;

    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_EtConfig)
};

struct S2CCIMDegrade : public S2CCIMStatusBase {
    DECLARE_MSG()
    IDENTIFIER(S2C_CIM_Degrade)
};




#endif  // MESSAGE_OBJECT_H_
