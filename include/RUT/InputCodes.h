#pragma once

#include<cstdint>

namespace rut
{
    enum KeyCode : uint16_t
    {
        KC_A,
        KC_B,
        KC_C,
        KC_D,
        KC_E,
        KC_F,
        KC_G,
        KC_H,
        KC_I,
        KC_J,
        KC_K,
        KC_L,
        KC_M,
        KC_N,
        KC_O,
        KC_P,
        KC_Q,
        KC_R,
        KC_S,
        KC_T,
        KC_U,
        KC_V,
        KC_W,
        KC_X,
        KC_Y,
        KC_Z,
        KC_0,
        KC_1,
        KC_2,
        KC_3,
        KC_4,
        KC_5,
        KC_6,
        KC_7,
        KC_8,
        KC_9,
        KC_SPACE,
        KC_ENTER,
        KC_BACKSPACE,
        KC_TAB,
        KC_CAPS_LOCK,
        KC_LEFT_SHIFT,
        KC_RIGHT_SHIFT,
        KC_LEFT_CTRL,
        KC_RIGHT_CTRL,
        KC_LEFT_ALT,
        KC_RIGHT_ALT,
        KC_HOME,
        KC_LEFT,
        KC_RIGHT,
        KC_DOWN,
        KC_UP,
        KC_ESCAPE,
        KC_INSERT,
        KC_LAST
    };

    enum MouseButtonCode : uint8_t
    {
        MBC_LEFT,
        MBC_MIDDLE,
        MBC_RIGHT,
        MBC_LAST
    };
}