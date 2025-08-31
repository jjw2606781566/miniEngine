#include "CameraController.h"
#include "Engine/Utility/MacroUtility.h"
#include "Engine/InputControl/TankinInput.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utility/GameTime/GameTime.h"

REGISTER_COMPONENT(CameraController,"CameraController")

void CameraController::awake()
{
    
}

void CameraController::start()
{
    mCamera = dynamic_cast<Camera*>(mGameObject->getComponent("Camera"));
    ASSERT(mCamera != nullptr, TEXT("mCamera is nullptr!"));
    mTransform = mGameObject->getTransform();
    DEBUG_PRINT_START(mGameObject->getName().c_str(),"CameraController");
}

void CameraController::update()
{
    if (Application::sGetRunningType() == EngineRunningType::Editor ||
        Application::sGetRunningType() == EngineRunningType::Debug)
    {
        EditorOperation();
    }
}

void CameraController::EditorOperation()
{
    DEBUG_PRINT_UPDATE(mGameObject->getName().c_str(), "CameraController");
    float moveSpeed = 10.0f;
    Vector3 forward = mTransform->getForward();
    Vector3 right = mTransform->getRight();
    Vector3 position = mTransform->getWorldPosition();

    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_LEFT_SHIFT))
    {
        moveSpeed = 40.0f;
    }
    else
    {
        moveSpeed = 10.0f;
    }
    
    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_A))
    {
        position -= right* static_cast<float>(moveSpeed * GameTime::sGetDeltaTime());
        mTransform->setWorldPosition(position);
    }
    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_D))
    {
        position += right*static_cast<float>(moveSpeed * GameTime::sGetDeltaTime());
        mTransform->setWorldPosition(position);
    }
    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_S))
    {
        position -= forward*static_cast<float>(moveSpeed * GameTime::sGetDeltaTime());
        mTransform->setWorldPosition(position);
    }
    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_W))
    {
        position += forward*static_cast<float>(moveSpeed * GameTime::sGetDeltaTime());
        mTransform->setWorldPosition(position);
    }
    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_E))
    {
        position.v.y += static_cast<float>(moveSpeed * GameTime::sGetDeltaTime());
        mTransform->setWorldPosition(position);
    }
    if (TankinInput::sGetInstance()->isKeyPressed(KeyCode::KC_Q))
    {
        position.v.y -= static_cast<float>(moveSpeed * GameTime::sGetDeltaTime());
        mTransform->setWorldPosition(position);
    }
    
    float rotationSpeed = 1.0f;
    Vector2 mouseMotion = TankinInput::sGetInstance()->getMouseMotion();
    mTransform->rotateAroundLocalAxis({1,0,0}
        , static_cast<float>(mouseMotion.v.y*GameTime::sGetDeltaTime()*rotationSpeed * MathUtils::DEG_TO_RAD));
    mTransform->rotateAroundWorldAxis({0,1,0}
        , static_cast<float>(mouseMotion.v.x*GameTime::sGetDeltaTime()*rotationSpeed * MathUtils::DEG_TO_RAD));
}
