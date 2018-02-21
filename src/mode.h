#ifndef MODE_H
#define MODE_H

class SetupMode;
class SimulationMode;
class VideoMode;

struct Gymnast;

class Mode
{
  public:
    enum Type {
        SETUP, SIMULATION, VIDEO
    };
    Type type;

  public:
    static SetupMode      *setup_mode;
    static SimulationMode *simulation_mode;
    static VideoMode      *video_mode;
    static void set_gymnast (Gymnast *g) { gymnast = g; }

  public:
    Mode(Type t) : type(t) {}
    virtual Mode *transition(char c) = 0;
    virtual void enter() = 0;

  protected:
    static Gymnast *gymnast;
};

class SetupMode : public Mode
{
  public:
    SetupMode() : Mode(SETUP) {}
    Mode *transition(char c);
    void enter();
};

class SimulationMode : public Mode
{
  public:
    SimulationMode() : Mode(SIMULATION) {}
    Mode *transition(char c);
    void enter();
};

class VideoMode : public Mode
{
  public:
    VideoMode() : Mode(VIDEO) {}
    Mode *transition(char c);
    void enter();
};

#endif
