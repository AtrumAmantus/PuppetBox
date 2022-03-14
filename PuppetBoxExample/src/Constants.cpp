#include "Constants.h"

#include <puppetbox/KeyCode.h>

namespace Constants
{
    namespace Font
    {
        std::string kMochiyPop = "Assets1/Fonts/MochiyPop/Regular";
    }

    namespace Animation
    {
        namespace Pack
        {
            std::string kBasicHuman = "Assets1/Animations/BasicHuman";
        }

        std::string kWalk = Pack::kBasicHuman + "/Walk";
        std::string kIdle0 = Pack::kBasicHuman + "/Idle_0";
    }

    namespace Input
    {
        std::uint8_t inputKeys[] = {
                KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
                KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
                KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL,
                KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, KEY_BACKSLASH, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_COMMA,
                KEY_PERIOD, KEY_SLASH, KEY_SPACE, KEY_KP_DIVIDE, KEY_KP_MULTIPLY, KEY_KP_SUBTRACT, KEY_KP_ADD,
                KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9,
                KEY_KP_0, KEY_KP_DECIMAL, KEY_GRAVE_ACCENT
        };

        std::uint32_t inputKeyCount = 63;
    }
}