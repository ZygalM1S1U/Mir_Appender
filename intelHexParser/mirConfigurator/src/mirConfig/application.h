#ifndef _APPLICATION_H__
#define _APPLICATION_H__

#include <SDL.h>
#include <vector>
#include "msg_event.h"
#include "singleton.h"
#include "menu_config_component.h"

// forward declares
class AppComponent;
class MenuConfigComponent;

#define MAX_STATE_FRAMES 100

#define REVISION_MAJOR "0"
#define REVISION_MINOR "0"
#define REVISION_BUILD "1"

class Application : public Singleton<Application>
{
public:

    /// @brief  constructor
    Application();

    /// @brief  destructor
    virtual ~Application();

    /// @brief setup GUI framework and scan for COM ports
    bool init();

    /// @brief update all GUI widgets and variables
    void run();

    void beginShutdown() { done = true; }

    MsgEvent<bool> Event_TimerEveryFiveSec;
    MsgEvent<bool> Event_TimerOnePerSec;
    MsgEvent<bool> Event_TimerFourPerSec;
    MsgEvent<bool> Event_TimerTwoPerSec;
    MsgEvent<bool> Event_Timer10ms;

    MsgEvent<float> Event_Draw;


protected:

    /// @brief Setup SDL window with graphics support
    bool initSDL();

    /// @brief cleanup GUI framework
    void shutdown();

    SDL_Window*   window;
    SDL_GLContext glcontext;

    // app components
    MenuConfigComponent* menu;

    uint64_t currTime = 0;
    uint64_t lastTime = 0;

    bool done = false;

    // test variables to plot FPS for an example
    float maxFPS = 0;
    float minFPS = 1000;
    float data[MAX_STATE_FRAMES];
    float counter = 0;

    float tick_1;
    float tick_2;
    float tick_4;
    float tick_50ms;
    float tick_5sec;
};

#endif
