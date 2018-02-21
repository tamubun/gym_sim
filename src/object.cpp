#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <map>
#include <iostream>

using namespace std;

#include "object.h"
#include "addition.h"

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

// select correct drawing functions
#ifdef dDOUBLE
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawBox dsDrawBoxD
#define aDrawEllipsoid aDrawEllipsoidD
#endif

dWorldID Object::world = 0;

void
Object::world_init()
{
    if ( world != 0 ) {
        dWorldDestroy(world);
    }

    world = dWorldCreate();
    dWorldSetERP(world, .6);
    dWorldSetCFM(world, .01);
}

Object::Object(const char *n, float red, float green, float blue)
        : name(n), hinge(0)
{
    id = dBodyCreate(world);
    color[0] = red; color[1] = green; color[2] = blue;
}

void
Object::connect(const Object *other, dReal y, dReal z,
                dReal hiStop, dReal loStop)
{
    if ( ! hinge )
        hinge = dJointCreateHinge(world, 0);
    dJointAttach(hinge, id, other ? other->id : 0);
    dJointSetHingeAnchor(hinge, 0, y, z);
    dJointSetHingeAxis(hinge, 1, 0, 0);
    if ( other == 0 || hiStop < 0 )
        return;
    dJointSetHingeParam(hinge, dParamHiStop, RAD(hiStop));
    dJointSetHingeParam(hinge, dParamLoStop, RAD(loStop));
    dJointSetHingeParam(hinge, dParamCFM, .01);
    dJointSetHingeParam(hinge, dParamVel, 0);
    dJointSetHingeParam(hinge, dParamFMax, 0.001);
}

void
Object::save_state()
{
    const dReal *vect;
    unsigned j;
    unsigned idx = 0;

    vect = dBodyGetPosition(id);
    for ( j = 0; j < 3; ++j )
        state[idx++] = vect[j];
    vect = dBodyGetRotation(id);
    for ( j = 0; j < 12; ++j )
        state[idx++] = vect[j];
    vect = dBodyGetLinearVel(id);
    for ( j = 0; j < 3; ++j )
        state[idx++] = vect[j];
    vect = dBodyGetAngularVel(id);
    for ( j = 0; j < 3; ++j )
        state[idx++] = vect[j];
}

void
Object::dump_state()
{
    const dReal *vect;
    dVector3 vect2;
    unsigned j;

    cout << name << ':' << endl;
    vect = dBodyGetPosition(id);
    cout << "pos = (";
    for ( j = 0; j < 3; ++j ) {
        cout << vect[j];
        if ( j < 2 )
            cout << ", ";
    }
    cout << ")\nrot = (";
    vect = dBodyGetRotation(id);
    for ( j = 0; j < 12; ++j ) {
        cout << vect[j];
        if ( j < 11 )
            cout << ", ";
    }
    cout << ")\nvel = (";
    vect = dBodyGetLinearVel(id);
    for ( j = 0; j < 3; ++j ) {
        cout << vect[j];
        if ( j < 2 )
            cout << ", ";
    }
    cout << ")\nangular vel= (";
    vect = dBodyGetAngularVel(id);
    for ( j = 0; j < 3; ++j ) {
        cout << vect[j];
        if ( j < 2 )
            cout << ", ";
    }
    if ( hinge != 0 ) {
        cout << ")\nhinge = (";
        dJointGetHingeAnchor(hinge, vect2);
        for ( j = 0; j < 3; ++j ) {
            cout << vect2[j];
            if ( j < 2 )
                cout << ", ";
        }
    }
    cout << ")" << endl;
}

void
Object::restore_state()
{
    unsigned j;
    unsigned idx = 0;
    dReal x,y,z;
    dMatrix3 rot;

    x = state[idx++]; y = state[idx++]; z = state[idx++];
    dBodySetPosition(id, x,y,z);
    for ( j = 0; j < 12; ++j )
        rot[j] = state[idx++];
    dBodySetRotation(id, rot);
    x = state[idx++]; y = state[idx++]; z = state[idx++];
    dBodySetLinearVel(id, x,y,z);
    x = state[idx++]; y = state[idx++]; z = state[idx++];
    dBodySetAngularVel(id, x,y,z);
}

Bar::Bar(float red, float green, float blue,
         dReal _mass, dReal _len, dReal _r, dReal _high,
         dReal _kp, dReal _kd, dReal _kf)
        : Object("Bar", red, green, blue),
          mass(_mass), len(_len), r(_r), high(_high), kp(_kp), kd(_kd), kf(_kf)
{
    dMass m;
    dMassSetCylinder(&m, 1, 3, r, len);
    dMassAdjust(&m, mass);
    dBodySetMass(id, &m);
    gid = dCreateCCylinder(0, r, len);
    dGeomSetBody(gid, id);

    dQuaternion q;
    dQFromAxisAndAngle(q, 0, 1,0, M_PI/2);
    dBodySetQuaternion(id, q);
    dBodySetPosition(id, 0, 0, high);
    volume = M_PI*r*r*len;

#if 0
    /* ‚±‚±‚ð“ü‚ê‚é‚ÆAƒo[‚Ì‚µ‚È‚è‚ª‚È‚­‚È‚é */
    hinge = dJointCreateFixed(world, 0);
    dJointAttach(hinge, id, 0);
    dJointSetFixed(hinge);
#endif

    feedback = new dJointFeedback();
    for ( unsigned i = 0; i < 3; ++i )
        feedback->f1[i] = feedback->f2[i] = feedback->t1[i] = feedback->t2[i] = 0;
}

void
Bar::getForce(dVector3 result)
{
    const dReal *pos = dBodyGetPosition(id);
    const dReal *vel = dBodyGetLinearVel(id);
    dVector3 tmp;
    dReal f;
    unsigned i;

    for ( i = 0; i < 3; ++i ) result[i] = 0;

    tmp[0] = 0; tmp[1] = pos[1]; tmp[2] = pos[2] - high;
    f = kp * dSqrt(dDot(tmp, tmp, 3));
    if ( f > 0.0001 ) {
        dNormalize3(tmp);
        for ( i = 1; i < 3; ++i )
            result[i] = - f * tmp[i];
    }
    f = kd * dSqrt(dDot(vel, vel, 3));
    if ( f > 0.0001 ) {
        for ( i = 1; i < 3; ++i ) tmp[i] = vel[i];
        dNormalize3(tmp);
        for ( i = 1; i < 3; ++i )
            result[i] -= f * tmp[i];
    }
}

void
Bar::getTorque(dVector3 result, Object *obj)
{
    for ( unsigned i = 0; i < 3; ++i ) result[i] = 0;

    const dReal *omega = dBodyGetAngularVel(obj->id);
    result[0] = ( (omega[0] < 0) ? +1 : -1 ) *
        kf * dSqrt(dDot(feedback->f2, feedback->f2, 3));
}

void
Bar::draw()
{
    const dReal *pos = dBodyGetPosition(id);
    const dReal *rot = dBodyGetRotation(id);
    dVector3 vect, dir, tmp;
    dMatrix3 rot2;
    dReal l, root;
    unsigned i;

    dsSetColor(color[0], color[1], color[2]);
    dsDrawCylinder(pos, rot, len, r);

    vect[0] = len; vect[1] = pos[1]; vect[2] = pos[2] - high;
    l = dSqrt(dDot(vect, vect, 3));
    for ( i = 0; i < 3; ++i ) {
        dir[i] = vect[i] / l;
        rot2[3+4*i] = 0;
    }
    rot2[2] = dir[0]; rot2[6] = dir[1]; rot2[10] = dir[2];
    root = dSqrt(dir[0] * dir[0] + dir[1] * dir[1]);
    rot2[0] = -dir[1] / root; rot2[4] = dir[0] / root; rot2[8] = 0;
    dCROSS444(rot2+1, =, rot2, rot2 + 2);
    tmp[0] = -len; tmp[1] = vect[1]/2; tmp[2] = vect[2]/2 + high;
    dsSetColor(color[0], color[1], color[2]);
    dsDrawCylinder(tmp, rot2, l, r);

    rot2[1] = -rot2[1]; rot2[2] = -rot2[2];
    rot2[0] = -rot2[0];
    dCROSS444(rot2+1, =, rot2, rot2 + 2);
    tmp[0] += len*2;
    dsSetColor(color[0], color[1], color[2]);
    dsDrawCylinder(tmp, rot2, l, r);
}

Body::Body(const char *name, float red, float green, float blue,
           dReal _rho, dReal lx, dReal ly, dReal lz, dReal _top)
        : Object(name, red, green, blue), rho(_rho), top(_top)
{
    sides[0] = lx; sides[1] = ly; sides[2] = lz;

    dMass m;
    aMassSetEllipsoid(&m, rho, lx, ly, lz);
    dBodySetMass(id, &m);
    gid = dCreateBox(0, lx, ly, lz);
    dGeomSetBody(gid, id);

    dBodySetPosition(id, 0, 0, top-lz/2);
    volume = (1.0/6.0)*M_PI*lx*ly*lz;
}

void
Body::draw()
{
    dsSetColor(color[0], color[1], color[2]);
    dsSetTexture(DS_WOOD);
    aDrawEllipsoid(dBodyGetPosition(id), dBodyGetRotation(id), sides);
}

Leg::Leg(const char *name, float red, float green, float blue,
         dReal _rho, dReal _len, dReal _r, dReal _top, dReal _dist)
        : Object(name, red, green, blue), rho(_rho), len(_len), r(_r),
          top(_top), dist(_dist)
{
    dMass m, m1;
    dMassSetCylinder(&m, rho, 3, r, len);
    m1 = m;
    dMassTranslate(&m, -dist/2, 0, 0);
    dMassTranslate(&m1, dist/2, 0, 0);
    dMassAdd(&m, &m1);
    dBodySetMass(id, &m);
    gid = dCreateBox(0, dist + r*2, r*2, len);
    dGeomSetBody(gid, id);

    dBodySetPosition(id, 0, 0, top-len/2);
    volume = 2.0 * M_PI * r * r * len;
}

void
Leg::draw()
{
    const dReal *pos = dBodyGetPosition(id);
    dVector3 pos1;

    for ( unsigned i = 0; i < 3; ++i ) pos1[i] = pos[i];
    dsSetColor(color[0], color[1], color[2]);
    dsSetTexture(DS_WOOD);
    pos1[0] -= dist/2;
    dsDrawCylinder(pos1, dBodyGetRotation(id), len, r);
    pos1[0] += dist;
    dsDrawCylinder(pos1, dBodyGetRotation(id), len, r);
}
