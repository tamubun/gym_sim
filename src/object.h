#ifndef OBJECT_H
#define OBJECT_H

#include <ode/ode.h>
#include <vector>
#include <string>
using namespace std;

/* GRAVITY: 重力加速度。
   シミュレーション世界の時間単位を1[u]とすると、
   実世界の1[秒]は、sqrt(9.81/GRAVITY) [u] (約18.1[u])。
   シミュレーションは、stepsize [u]ごとに計算される。
   力の単位も、[kg][m]/[u^2] */
#define GRAVITY (0.03)
#define RAD(degree) ( (degree) / 180. * M_PI )

struct Object
{
    static dWorldID world;
    static void world_init();

    const char *name;
    dBodyID id;
    dJointID hinge;
    dReal volume;
    float color[3];
    dReal state[3*3+12];
    dGeomID gid;

    Object(const char *n, float red, float green, float blue);

    /* hiStop, loStopは degreeで指定
       hiStop < 0 なら、制限なし */
    void connect(const Object *other, dReal y, dReal z,
                 dReal hiStop, dReal loStop);

    virtual void draw() = 0;

    void save_state();
    void dump_state();
    void restore_state();
};

struct Bar : public Object
{
    dReal mass, len, r;
    dReal high;
    dReal kp, kd, kf;
    dJointFeedback *feedback;

    Bar(float red, float green, float blue,
        dReal _mass, dReal _len, dReal _r, dReal _high,
        dReal _kp, dReal _kd, dReal _kf);

    void getForce(dVector3 result);

    void getTorque(dVector3 result, Object *obj);

    void draw();
};

struct Body : public Object
{
    dReal rho, top;
    dReal sides[3];

    Body(const char *name, float red, float green, float blue,
         dReal _rho, dReal lx, dReal ly, dReal lz, dReal _top);

    void draw();
};

struct Leg : public Object
{
    dReal rho, len, r, top;
    dReal dist; // 左右の幅

    Leg(const char *name, float red, float green, float blue,
        dReal _rho, dReal _len, dReal _r, dReal _top, dReal _dist);

    void draw();
};

#endif
