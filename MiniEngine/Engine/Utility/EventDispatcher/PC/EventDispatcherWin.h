#pragma once
#ifdef WIN32
#include "../EventDispatcher.h"

class EventDispatcherWin : public IEventDispatcher
{
public:
    bool Dispatch() override;
    void PostEvent(EventType eventId) override;
};
#endif