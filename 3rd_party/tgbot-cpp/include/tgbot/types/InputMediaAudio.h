#ifndef TGBOT_INPUTMEDIAAUDIO_H
#define TGBOT_INPUTMEDIAAUDIO_H

#include <memory>
#include <string>

#include "tgbot/types/InputMedia.h"

namespace TgBot {

/**
 * @brief Represents a video to be sent.
 * @ingroup types
 */
class InputMediaAudio : public InputMedia {
public:
    typedef std::shared_ptr<InputMediaAudio> Ptr;

    InputMediaAudio() {
        this->type = TYPE::AUDIO;
    }
};
}

#endif //TGBOT_INPUTMEDIAAUDIO_H
