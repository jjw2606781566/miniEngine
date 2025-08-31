#include "TankinInput.h"

#ifdef WIN32
#include "PC/InputHandlerWin.h"
#endif
#ifdef ORBIS
#include "PS/InputHandlerORBIS.h"
#endif

TankinInput* TankinInput::sInstance = nullptr;

void TankinInput::sInit()
{
#ifdef WIN32
    InputHandlerWIN* inputHandler = new InputHandlerWIN();
    sInstance = static_cast<TankinInput*>(inputHandler);
    //按键绑定
    sInstance->bindKey(Action::MOVE_FORWARD, KC_W);
    sInstance->bindKey(Action::MOVE_BACK, KC_S);
    sInstance->bindKey(Action::CAMERA_TURN_DOWN, KC_K);
    sInstance->bindKey(Action::CAMERA_TURN_UP, KC_I);
    sInstance->bindKey(Action::CAMERA_TURN_LEFT, KC_J);
    sInstance->bindKey(Action::CAMERA_TURN_RIGHT, KC_L);
    sInstance->bindKey(Action::CAMERA_FAR, KC_Q);
    sInstance->bindKey(Action::CAMERA_NEAR, KC_E);
    sInstance->bindKey(Action::FIRE, KC_SPACE);
    sInstance->bindKey(Action::CANCEL, KC_ESCAPE);
    sInstance->bindKey(Action::CONFIRM, KC_ENTER);
    sInstance->bindKey(Action::OPTION, KC_ESCAPE);
    sInstance->bindKey(Action::SELECT_UP, KC_UP);
    sInstance->bindKey(Action::SELECT_DOWN, KC_DOWN);
    sInstance->bindKey(Action::SELECT_LEFT, KC_LEFT);
    sInstance->bindKey(Action::SELECT_RIGHT, KC_RIGHT);

    inputHandler->bindAxisMap(AxisKeyCode::LEFT_STICK_DOWN, KeyCode::KC_S);
    inputHandler->bindAxisMap(AxisKeyCode::LEFT_STICK_UP, KeyCode::KC_W);
    inputHandler->bindAxisMap(AxisKeyCode::LEFT_STICK_RIGHT, KeyCode::KC_D);
    inputHandler->bindAxisMap(AxisKeyCode::LEFT_STICK_LEFT, KeyCode::KC_A);
    inputHandler->bindAxisMap(AxisKeyCode::RIGHT_STICK_LEFT, KeyCode::KC_J);
    inputHandler->bindAxisMap(AxisKeyCode::RIGHT_STICK_RIGHT, KeyCode::KC_L);
    inputHandler->bindAxisMap(AxisKeyCode::RIGHT_STICK_UP, KeyCode::KC_I);
    inputHandler->bindAxisMap(AxisKeyCode::RIGHT_STICK_DOWN, KeyCode::KC_K);
#endif
#ifdef ORBIS
    sInstance = static_cast<TankinInput*>(new InputHandlerORBIS());
    //按键绑定
    sInstance->bindKey(Action::MOVE_FORWARD, KeyCode::KC_R2);
    sInstance->bindKey(Action::MOVE_BACK, KeyCode::KC_L2);

    

    sInstance->bindKey(Action::CAMERA_FAR, KeyCode::KC_L1);
    sInstance->bindKey(Action::CAMERA_NEAR, KeyCode::KC_R1);
    sInstance->bindKey(Action::FIRE, KeyCode::KC_CROSS);
    sInstance->bindKey(Action::CANCEL, KeyCode::KC_CIRCLE);
    sInstance->bindKey(Action::CONFIRM, KeyCode::KC_CROSS);
    sInstance->bindKey(Action::OPTION, KeyCode::KC_OPTIONS);
    sInstance->bindKey(Action::SELECT_UP, KeyCode::KC_UP);
    sInstance->bindKey(Action::SELECT_DOWN, KeyCode::KC_DOWN);
    sInstance->bindKey(Action::SELECT_LEFT, KeyCode::KC_LEFT);
    sInstance->bindKey(Action::SELECT_RIGHT, KeyCode::KC_RIGHT);

#endif
}

TankinInput* TankinInput::sGetInstance()
{
#ifdef WIN32
    return sInstance;
#endif
#ifdef ORBIS
    return sInstance;
#endif
}

bool TankinInput::isKeyDown(KeyCode key)
{
    return false;
}

bool TankinInput::isKeyHeld(KeyCode key)
{
    return false;
}

bool TankinInput::isKeyPressed(KeyCode key)
{
    return false;
}

bool TankinInput::isKeyUp(KeyCode key)
{
    return false;
}


void TankinInput::update()
{
}

void TankinInput::bindKey(Action action, KeyCode key)
{
    mActionKeyMap[action] = key;
}

bool TankinInput::isKeyDown(Action action)
{
    auto it = mActionKeyMap.find(action);
    if (it != mActionKeyMap.end()) {
        return isKeyDown(it->second);
    }
    return false;
}

bool TankinInput::isKeyHeld(Action action)
{
    auto it = mActionKeyMap.find(action);
    if (it != mActionKeyMap.end()) {
        return isKeyHeld(it->second);
    }
    return false;
}

bool TankinInput::isKeyPressed(Action action)
{
    auto it = mActionKeyMap.find(action);
    if (it != mActionKeyMap.end()) {
        return isKeyPressed(it->second);
    }
    return false;
}

bool TankinInput::isKeyUp(Action action)
{
    auto it = mActionKeyMap.find(action);
    if (it != mActionKeyMap.end()) {
        return isKeyUp(it->second);
    }
    return false;
}

TankinInput::TankinInput() = default;
