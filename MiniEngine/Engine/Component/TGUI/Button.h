#pragma once
#include "ImageTGUI.h"
#include "Engine/Application.h"

class Button :public ImageTGUI
{
public:
    void awake() override;
    static Button* sGetCurrentSelectedButton(){return sCurrentSelectedButton;}
    //call back
    void addOnClickEvent(const std::function<void(Button* thisButton)>& func) { mOnClickEvents.push_back(func); }
    void addOnHoverEvent(const std::function<void(Button* thisButton)>& func) { mOnHoverEvents.push_back(func); }
    void addOnUnHoverEvent(const std::function<void(Button* thisButton)>& func) { mOnUnHoverEvents.push_back(func); }

    void clearOnClickEvent() {mOnClickEvents.clear();}
    void clearOnHoverEvent() {mOnHoverEvents.clear();}
    void clearOnUnHoverEvent() {mOnUnHoverEvents.clear();}

    //select
    void setNextButton(Button* button){mNextButton = button;}
    void setLastButton(Button* button){mLastButton = button;}

    static void sSetCurrentButton(Button* button)
    {
        if (sCurrentSelectedButton != nullptr)
            sCurrentSelectedButton->unHover();
        sCurrentSelectedButton = button;
        sCurrentSelectedButton->hover();
    }
    
    //Operation
    static void sTrigger()
    {
        if (Application::sGetRunningType() == EngineRunningType::Editor)
            return;
        if (sCurrentSelectedButton == nullptr)
            return;
        sCurrentSelectedButton->click();
    }
    static void sNext()
    {
        if (sCurrentSelectedButton == nullptr)
            return;
        if (sCurrentSelectedButton->mNextButton == nullptr)
            return;
        sCurrentSelectedButton->unHover();
        sCurrentSelectedButton = sCurrentSelectedButton->mNextButton;
        sCurrentSelectedButton->hover();
    }
    static void sLast()
    {
        if (sCurrentSelectedButton == nullptr)
            return;
        if (sCurrentSelectedButton->mLastButton == nullptr)
            return;
        sCurrentSelectedButton->unHover();
        sCurrentSelectedButton = sCurrentSelectedButton->mLastButton;
        sCurrentSelectedButton->hover();
    }
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    virtual ~Button() override;

private:
    void click()
    {
        for (auto& func:mOnClickEvents)
            func(this);
    }
    void hover()
    {
        for (auto& func:mOnHoverEvents)
            func(this);
    }
    void unHover()
    {
        for (auto& func:mOnUnHoverEvents)
            func(this);
    }
    
    TpList<std::function<void(Button* thisButton)>> mOnClickEvents;
    TpList<std::function<void(Button* thisButton)>> mOnHoverEvents;
    TpList<std::function<void(Button* thisButton)>> mOnUnHoverEvents;
    Button* mLastButton = nullptr;
    Button* mNextButton = nullptr;

    static Button* sCurrentSelectedButton;
};
