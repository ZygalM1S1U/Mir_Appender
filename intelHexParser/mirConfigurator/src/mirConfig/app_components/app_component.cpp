#include "app_component.h"
#include "application.h"

AppComponent::AppComponent()
{
    // subscribe to draw event
    drawSubscriptionID = Application::getSingletonPtr()->Event_Draw.subscribe(this, &AppComponent::handleDrawEvent);
}

AppComponent::~AppComponent()
{
    // unsubscribe draw event
    Application::getSingletonPtr()->Event_Draw.unsubscribe(drawSubscriptionID);
}

void AppComponent::handleDrawEvent(float elapsedTime)
{
    draw(elapsedTime);
}
