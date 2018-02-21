#include <cstdio>
using namespace std;

#include "addition.h"
#include "gymnast.h"
#include "mode.h"

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

SetupMode *Mode::setup_mode = new SetupMode();
SimulationMode *Mode::simulation_mode = new SimulationMode();
VideoMode *Mode::video_mode = new VideoMode();

Gymnast *Mode::gymnast = 0;

Mode *
SetupMode::transition(char c)
{
    Mode *mode;

    switch (c) {
    case ' ':
        mode = simulation_mode;
        break;
    case 'V':
        mode = video_mode;
        break;
    default:
        mode = setup_mode;
        break;
    }

    mode->enter();
    return mode;
}

Mode *
SimulationMode::transition(char c)
{
    Mode *mode;

    switch (c) {
    case 'R':
        mode = setup_mode;
        break;
    case 'V':
        mode = video_mode;
        break;
    default:
        mode =  simulation_mode;
        break;
    }

    mode->enter();
    return mode;
}

Mode *
VideoMode::transition(char c)
{
    Mode *mode;

    switch (c) {
    case 'R':
        mode = setup_mode;
        break;
    default:
        mode = video_mode;
        break;
    }

    mode->enter();
    return mode;
}

void
SetupMode::enter()
{
    printf("\n**** Setup Mode ****\n\n");

    printf("'V': Video Mode.\n");
    printf("' ': Simulation Start.\n\n");

    printf("'R': Reset.\n");
    printf("'a/s': Kouhou/Zenpou kaiten.\n");
    printf("'q/w': Kouhou/Zenpou kaiten (bichousei).\n");

    aSetPause(1);
}

void
VideoMode::enter()
{
    printf("\n**** Video Mode ****\n\n");
    printf("'R': Reset and enter Setup Mode.\n\n");

    printf("' ': Review start.\n");
    printf("'L': Load.\n");
    printf("'S': Save.\n");

    aSetPause(1);
}

void
SimulationMode::enter()
{
    printf("\n**** Simulation Mode ****\n\n");
    printf("'R': Reset and enter Setup Mode.\n\n");
    printf("'V': Video Mode.\n\n");

    printf("' ': Pause/Unpause.\n");
    printf("'o': Release bar.\n");
    printf("'+/-': Adjust force.\n");
    gymnast->print_message();

    aSetPause(0);
}
