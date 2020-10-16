#ifndef _APP_COMPONENT_H_
#define _APP_COMPONENT_H_

#include "msg_event.h"

class AppComponent
{
public:

    AppComponent();
    virtual ~AppComponent();

    virtual void draw(float elapsedTime) = 0;

    void handleDrawEvent(float elapsedTime);

protected:

    MsgEventHandler drawSubscriptionID;
};

#endif
