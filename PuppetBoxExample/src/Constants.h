#pragma once

#include <string>

#include <PuppetBox.h>

namespace Constants
{
    namespace Font
    {
        std::string MochiyPop = "Assets1/Fonts/MochiyPop/Regular";
    }

    namespace Input
    {
        std::uint8_t inputKeys[] = {
            KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
            KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
            KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_SPACE, KEY_COMMA,
            KEY_APOSTROPHE, KEY_PERIOD,
        };

        std::uint8_t inputKeyValues[] = {
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
                't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
                '3', '4', '5', '6', '7', '8', '9', '0', ' ',
                ',', '\'', '.',
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                'U', 'V', 'W', 'X', 'Y', 'Z', '!', '@', '#', '$',
                '%', '^', '&', '*', '(', ')', ' ', '<', '"', '>',
        };

        std::uint32_t inputKeyCount = sizeof(inputKeys);
    }
}