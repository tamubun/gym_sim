#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <map>

using namespace std;

#include "object.h"
#include "gymnast.h"
#include "movement.h"

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

static map<string, Movement*> movement_map;
static Kansetsu current[Movement::POS_END];

Gymnast::Gymnast(dReal high) : cur_movement(0)
{
    /* ˜e‚Ì‰Â“®ˆæ‚ð -5K` +270K“™‚ÉŽw’è‚µ‚½‚¢‚ªAhinge stop‚Ì”ÍˆÍ‚Í
       -180K` +180K‚ÅŽw’è‚µ‚È‚¢‚Æ‚¢‚¯‚È‚¢‚Ì‚Å‚â‚â‚±‚µ‚¢ */
# if 0 // ‚â‚è‚½‚¢‚±‚Æ
    arm = new Leg(1,1,0, 1050, .7, .045, high, .30);
    mune =
        new Body(3,3,3, 950, .31, .21, .2, high - .95 * arm->len);
    mune->connect(arm, 0, high - .975 * arm->len, 270, -5);
# else // ‚ß‚ñ‚Ç‚­‚³‚¢
    /* ˜r‚ÌŠp“x‚ÍA‰”’¼‚©‚ç30“x‘O‚ÉŠJ‚¢‚½ˆÊ’u‚ðŠî€‚É‚µ‚ÄŒv‚é*/
    arm = new Leg("Ude", 1,1,0, 1050, .7, .045, 0, .30);
    dQuaternion q;
    dQFromAxisAndAngle(q, 1,0,0, RAD(-150.));
    dBodySetPosition(arm->id, 0,0,0);
    dBodySetQuaternion(arm->id, q);
    mune = new Body("Mune", 3,3,3, 950, .31, .21, .2, 0);
    dReal xx = .475 * arm->len;
    dReal anchor_y = - xx * sin(RAD(30.));
    dReal anchor_z = + xx * cos(RAD(30.));
    dBodySetPosition(mune->id, 0, anchor_y,
                     + anchor_z + .025*arm->len - .5*mune->sides[2]);
    mune->connect(arm, anchor_y, anchor_z, 120, -155);

    dQFromAxisAndAngle(q, 1,0,0, 0);
    dBodySetQuaternion(arm->id, q);
    dBodySetPosition(arm->id, 0, 0, high - .5*arm->len);
    dBodySetPosition(mune->id, 0, 0, high - .95 * arm->len - .5*mune->sides[2]);
# endif

    hara =
        new Body("Hara", 3,3,3, 950, .28, .18, .2,
                 high - .95 * (arm->len + mune->sides[2]));
    koshi =
        new Body("Koshi", 0,0,3, 950, .32, .20, .2,
                 high - .95 * (arm->len + mune->sides[2] + hara->sides[2]));
    leg1 =
        new Leg("Momo", 1,1,0, 1050, .50, .080,
                high - .95 * (arm->len + mune->sides[2] + hara->sides[2] +
                              koshi->sides[2]), .16);
    leg2 =
        new Leg("Sune", 1,1,0, 1050, .60, .05,
                high - .95 * (arm->len + mune->sides[2] + hara->sides[2] +
                              koshi->sides[2]) - leg1->len, .13);
    atama =
        new Body("Dotama", 1,1,0, 1050, .18, .22, .28,
                 high - .95 * (arm->len - .28));

    arm->connect(0, 0, high, -1, -1); // setup_phase‚ÌŒã‚Ébar‚ÆŒq‚®
    atama->connect(mune, 0, high - .975 * arm->len, 50, -70);
    hara->connect(mune, 0, high - .975 * (arm->len + mune->sides[2]), 45, -15);
    koshi->connect(hara, 0, high - .975 * (arm->len + mune->sides[2] + hara->sides[2]),
                   60, -25);
    leg1->connect(koshi, 0,
                  high - .975 * (arm->len + mune->sides[2] + hara->sides[2] +
                                 koshi->sides[2]),
                  135, -35);
    leg2->connect(leg1, 0,
                  high - .975 * (arm->len + mune->sides[2] + hara->sides[2] +
                                 koshi->sides[2]) - leg1->len,
                  1, -160);

    parts.push_back(atama);
    parts.push_back(mune);
    parts.push_back(hara);
    parts.push_back(koshi);
    parts.push_back(arm);
    parts.push_back(leg1);
    parts.push_back(leg2);

    jgid = dJointGroupCreate(0);

    space = dSimpleSpaceCreate(0);
    dSpaceAdd(space, atama->gid);
    dSpaceAdd(space, mune->gid);
    dSpaceAdd(space, hara->gid);
    dSpaceAdd(space, koshi->gid);
    dSpaceAdd(space, arm->gid);
    dSpaceAdd(space, leg1->gid);
    dSpaceAdd(space, leg2->gid);

    weight = 0;
    dMass mass;
    for ( unsigned i = 0; i < parts.size(); ++i ) {
        dBodyGetMass(parts[i]->id, &mass);
        weight += mass.mass;
    }
}

Gymnast::~Gymnast()
{
    for (unsigned i = 0; i < parts.size(); ++i )
        delete parts[i];
}

void
Gymnast::draw()
{
    for (unsigned i = 0; i < parts.size(); ++i )
        parts[i]->draw();
}

void
Gymnast::stop(bool linear_only)
{
    for (unsigned i = 0; i < parts.size(); ++i ) {
        dBodySetLinearVel(parts[i]->id, 0,0,0);
        if ( !linear_only )
            dBodySetAngularVel(parts[i]->id, 0,0,0);
    }
}

void
Gymnast::install_movement(
    char key,
    string prefix,
    const string &message,

    dReal am, dReal am_vel, dReal am_fmax,     // Žñ‚ÌŠp“x
    dReal mu, dReal mu_vel, dReal mu_fmax,     // Œ¨Šp“x
    dReal hm, dReal hm_vel, dReal hm_fmax,     // ‹¹‚ÌŠJ‚«
    dReal kh, dReal kh_vel, dReal kh_fmax,     // ˜‚Ì”½‚è
    dReal l1k, dReal l1k_vel, dReal l1k_fmax,  // ‘«‚ÌŠp“x
    dReal l21, dReal l21_vel, dReal l21_fmax   // •GŠp“x
    )
{
    const dReal C = weight * GRAVITY;
    Movement *m = new Movement(message);

    if ( am != -dInfinity )
        m->push(Movement::KUBI,
                new Kansetsu(atama->hinge, RAD(am), am_vel, C * am_fmax));
    if ( mu != -dInfinity )
        m->push(Movement::KATA,
                new Kansetsu(mune->hinge, RAD(mu), mu_vel, C * mu_fmax));
    if ( hm != -dInfinity )
        m->push(Movement::MUNE,
                new Kansetsu(hara->hinge, RAD(hm), hm_vel, C * hm_fmax));
    if ( kh != -dInfinity )
        m->push(Movement::KOSHI,
                new Kansetsu(koshi->hinge, RAD(kh), kh_vel, C * kh_fmax));
    if ( l1k != -dInfinity )
        m->push(Movement::ASHI,
                new Kansetsu(leg1->hinge, RAD(l1k), l1k_vel, C * l1k_fmax));
    if ( l21 != -dInfinity )
        m->push(Movement::HIZA,
                new Kansetsu(leg2->hinge, RAD(l21), l21_vel, C * l21_fmax));

    movement_map[prefix + key] = m;
}

void
Gymnast::print_message()
{
    map<string, Movement*>::const_iterator it;

    for ( it = movement_map.begin(); it != movement_map.end(); ++it ) {
        Movement *move = (*it).second;
        if ( move->message == "" )
            continue;
        const string str = (*it).first;
        printf("'%c': %s\n", str[str.size()-1], move->message.c_str());
    }
}

void
Gymnast::dump_state()
{
    for ( unsigned i = 0; i < parts.size(); ++i )
        parts[i]->dump_state();        
}

void
Gymnast::set_movement(char key, string prefix)
{
    Movement *movement = 0;

    if ( movement_map.find(prefix + key) == movement_map.end() ) {
        string s = ""; s += key;
        if ( movement_map.find(s) != movement_map.end() ) {
            movement = movement_map[s];
        }
    }
    movement = movement_map[prefix + key];

    if ( movement == 0 )
        return;
    cur_movement = movement;

    for ( unsigned pos = 0; pos < Movement::POS_END; ++pos ) {
        if ( movement->array[pos] ) {
            current[pos] = *movement->array[pos];
            current[pos].start();
        }
    }
}

void
Gymnast::move()
{
#if 0
    static bool reach[Movement::POS_END];
#endif
    for ( unsigned pos = 0; pos < Movement::POS_END; ++pos ) {
        Kansetsu &k=current[pos];
        k.set_vel();
        if ( k.goal == dInfinity )
            continue;
#if 0
        if ( fabs(k.goal - dJointGetHingeAngle(k.joint)) < 0.15 ) {
            if ( !reach[pos] ) {
                printf("%d reach\n", pos);
                reach[pos] = true;
            }
        } else {
            if ( reach[pos] ) {
                printf("%d apart\n", pos);
                reach[pos] = false;
            }
        }
#endif
    }
}

void
Gymnast::plus()
{
    for ( unsigned pos = 0; pos < Movement::POS_END; ++pos ) {
        if ( cur_movement->array[pos] == 0 )
            continue;
        Kansetsu &k = current[pos];
        k.vel *= 1.1;
        k.fmax *= 1.2;
        k.start();
    }
}

void
Gymnast::minus()
{
    for ( unsigned pos = 0; pos < Movement::POS_END; ++pos ) {
        if ( cur_movement->array[pos] == 0 )
            continue;
        Kansetsu &k = current[pos];
        k.vel *= .9;
        k.fmax *= .8;
        k.start();
    }
}
