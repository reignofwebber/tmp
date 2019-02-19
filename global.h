#ifndef GLOBAL_H_
#define GLOBAL_H_

// std
#include <memory>

class Message;
class MessageObject;

class Session;
using SessionPtr = std::shared_ptr<Session>;

enum Message_Type {
    RealTime_Type,
    History_Type,
};

enum RuleSet {
    Unimplement_RuleSet,
};

enum Subscribe_Level {
    Subscribe_Area,
    Subscribe_Line,
    Subscribe_Device,
};

enum Playback_Control {
    Playback_Pause,
    Playback_Resume,
    Playback_Stop,
};


#endif  // GLOBAL_H_
