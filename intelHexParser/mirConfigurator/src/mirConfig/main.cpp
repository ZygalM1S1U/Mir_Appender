#include "application.h"

int main(int argc, char *args[])
{
    Application app;

    if (app.init())
        app.run();
    else
        return 1;

    return 0;
}