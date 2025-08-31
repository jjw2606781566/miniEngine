#pragma once
#include "Engine/Component/GameObject.h"
#include "Engine/Memory/TankinMemory.h"

struct Color;
class ShellController;
class GameObject;
class Buffable;
class Transform;

enum class BulletType : char{
    Common,
    Burn
};

class ShellFactory
{
public:
    static GameObject* sCreateCommonBullet(Transform* BatteryPointTransform, Buffable* buffable, const TpString& targetTag);
    static GameObject* sCreateBurnBullet(Transform* BatteryPointTransform, Buffable* buffable, const TpString& targetTag);
private:
    static GameObject* sCreateShell(Transform* BatteryPointTransform, const TpString& meshName,
        const TpString& textureName, const TpString& shaderName);
    static void sSetPhysics(GameObject* shell, Transform* BatteryPointTransform, bool isGravity = true);
    static ShellController* sSetController(GameObject* shell);
    static void sSetTailGas(ShellController* shellController);
    static GameObject* sSetBoom(ShellController* shellController, const Color& color);
    static void sSetAudio(GameObject* shellGo, GameObject* particleGo);
};
