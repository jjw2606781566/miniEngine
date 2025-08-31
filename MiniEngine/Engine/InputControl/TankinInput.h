#pragma once


#include "Engine/math/math.h"
#include "Engine/Utility/MacroUtility.h"

enum AxisKeyCode : char {
    LEFT_STICK_UP,
    LEFT_STICK_DOWN,
    LEFT_STICK_LEFT,
    LEFT_STICK_RIGHT,
    RIGHT_STICK_UP,
    RIGHT_STICK_DOWN,
    RIGHT_STICK_LEFT,
    RIGHT_STICK_RIGHT,
};

enum AxisType : char
{
	LEFT_STICK,
    RIGHT_STICK
};

#ifdef WIN32
enum KeyCode : uint32_t
{
    KC_MOUSE_LEFT = VK_LBUTTON,
    KC_MOUSE_RIGHT, 
    KC_MOUSE_MIDDLE = VK_MBUTTON,
    KC_ENTER = VK_RETURN,
    KC_PAUSE = VK_PAUSE,
    KC_ESCAPE = VK_ESCAPE,
    KC_SPACE = VK_SPACE,
    KC_LEFT = VK_LEFT,
    KC_UP,
    KC_RIGHT,
    KC_DOWN,
    KC_0 = 0x30,
    KC_1,
    KC_2,
    KC_3,
    KC_4,
    KC_5,
    KC_6,
    KC_7,
    KC_8,
    KC_9,
    KC_A = 0x41, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M, KC_N, KC_O, KC_P, KC_Q, KC_R,
    KC_S, KC_T, KC_U,
    KC_V, KC_W, KC_X, KC_Y, KC_Z,
    KC_NUMPAD_0 = 0x60,
    KC_NUMPAD_1,
    KC_NUMPAD_2,
    KC_NUMPAD_3,
    KC_NUMPAD_4,
    KC_NUMPAD_5,
    KC_NUMPAD_6,
    KC_NUMPAD_7,
    KC_NUMPAD_8,
    KC_NUMPAD_9,
    KC_F1 = 0x70, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12,
    KC_LEFT_SHIFT = VK_SHIFT,
    KC_RIGHT_SHIFT = VK_SHIFT,
    KC_LEFT_CTRL = VK_CONTROL,
    KC_RIGHT_CTRL = VK_CONTROL,
    KC_LEFT_ALT = VK_MENU,
    KC_RIGHT_ALT = VK_MENU,
};
#endif

#ifdef ORBIS
#include <pad.h>
enum class KeyCode : uint32_t {
    KC_CROSS = SCE_PAD_BUTTON_CROSS,
    KC_CIRCLE = SCE_PAD_BUTTON_CIRCLE,
    KC_SQUARE = SCE_PAD_BUTTON_SQUARE,
    KC_TRIANGLE = SCE_PAD_BUTTON_TRIANGLE,
    KC_L1 = SCE_PAD_BUTTON_L1,
    KC_R1 = SCE_PAD_BUTTON_R1,
    KC_L2 = SCE_PAD_BUTTON_L2,
    KC_R2 = SCE_PAD_BUTTON_R2,
    KC_L3 = SCE_PAD_BUTTON_L3,
    KC_R3 = SCE_PAD_BUTTON_R3,
    KC_OPTIONS = SCE_PAD_BUTTON_OPTIONS,
    KC_UP = SCE_PAD_BUTTON_UP,
    KC_DOWN = SCE_PAD_BUTTON_DOWN,
    KC_LEFT = SCE_PAD_BUTTON_LEFT,
    KC_RIGHT = SCE_PAD_BUTTON_RIGHT,
    KC_TOUCH_PAD = SCE_PAD_BUTTON_TOUCH_PAD
};
#endif

enum class Action
{
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_FORWARD,
    MOVE_BACK,
    CAMERA_TURN_RIGHT,
    CAMERA_TURN_LEFT,
    CAMERA_TURN_UP,
    CAMERA_TURN_DOWN,
    SELECT_UP,
    SELECT_DOWN,
    SELECT_RIGHT,
    SELECT_LEFT,
    CAMERA_NEAR,
    CAMERA_FAR,
    CONFIRM,
    OPTION,
    CANCEL,
    FIRE,
    PAUSE,
};

constexpr int8_t KEY_DOWN = 1;
constexpr int8_t KEY_HELD = 2;
constexpr int8_t KEY_UP = 3;

#ifdef WIN32
LRESULT InputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

class InputHandlerWIN;
class TankinInput
{
public:
    virtual ~TankinInput() = default;
    
    static void sInit();//create Input
    static TankinInput* sGetInstance();
#ifdef WIN32
    virtual Vector2 getMouseMotion() const = 0;
    virtual Vector2 getMousePosition() const = 0;
    virtual int16_t getWheelDelta() const = 0;
#endif
#ifdef ORBIS
    virtual Vector2 getRightStick() const = 0;
    virtual Vector2 getLeftStick() const = 0;
#endif
    virtual Vector2 getInputAxis(AxisType axisType) = 0;
    virtual bool isKeyDown(KeyCode key);
    virtual bool isKeyHeld(KeyCode key);
    virtual bool isKeyPressed(KeyCode key);
    virtual bool isKeyUp(KeyCode key);

    virtual void update();

    void bindKey(Action action, KeyCode key);
    bool isKeyDown(Action action);
    bool isKeyHeld(Action action);
    bool isKeyPressed(Action action);
    bool isKeyUp(Action action);
    
    
protected:
    DELETE_CONSTRUCTOR_FIVE(TankinInput);
    TankinInput();
    static TankinInput* sInstance;
private:
    std::unordered_map<Action, KeyCode> mActionKeyMap;
#ifdef WIN32
    friend LRESULT InputProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
};
