#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <ode/ode.h>
#include <vector>
using namespace std;
#include "gymnast.h"

struct Kansetsu
{
    dJointID joint;
    dReal goal;
    dReal vel;
    dReal fmax;

    /* fmax は、関節を動かすのに使うトルク。単位は、kg m / u^2 */
    Kansetsu(dJointID j = 0, dReal a =0 , dReal b = 0, dReal c = 0)
            : joint(j), goal(a), vel(b), fmax(c) {}

    void start() {
        dJointSetHingeParam(joint, dParamFMax, fmax);
        if ( goal == dInfinity )
            dJointSetHingeParam(joint, dParamVel, vel);
    }

    void set_vel() {
        if ( fmax == 0 || goal == dInfinity )
            return;
        dReal th = goal - dJointGetHingeAngle(joint);
        dJointSetHingeParam(joint, dParamVel, th*vel);
    }
};

struct Movement
{
    enum Pos {
        KUBI=0, KATA, MUNE, KOSHI, ASHI, HIZA, POS_END
    };

    string message;

    Kansetsu *array[POS_END];

    Movement(const string &mess) : message(mess) {
        for ( unsigned pos = 0; pos < POS_END; ++pos ) array[pos] = 0;
    }

    void push(Pos pos, Kansetsu *k) { array[pos] = k; }
};

#endif
