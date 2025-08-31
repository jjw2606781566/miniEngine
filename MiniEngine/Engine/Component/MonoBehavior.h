#pragma once
#include "Component.h"

//base class of gameplay script
class MonoBehavior:public Component
{
public:
    //Initialization
    virtual void awake() override;
    virtual void onEnable() override;
    virtual void start() override;

    //Physics
    virtual void fixedUpdate() override;

    //Gamelogic
    virtual void update() override;

    //Disable
    virtual void onDisable() override;

    //Decomissioning
    virtual void onDestory() override;

protected:
    
};
