/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

/* îPÇËÉVÉ~ÉÖÉåÅ[É^ ver 0.3
   by ÇΩÇﬁÇ‘ÇÒ
   http://club.pep.ne.jp/~tamubun/
   http://bunysmc.exblog.jp/
*/

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#else
#include <stdlib.h>
#include <string.h>
#endif

// select correct drawing functions
#ifdef dDOUBLE
#define dsDrawBox dsDrawBoxD
#define dsDrawLine dsDrawLineD
#define dsDrawSphere dsDrawSphereD
#endif

// some constants
#define SIZE (0.5f)

struct Parts
{
    dReal mass;     // èdÇ≥
    dReal width;    // â°ïù
    dReal depth;    // å˙Ç›
    dReal height;   // í∑Ç≥
};

const Parts parts_body =  {1.0, 1. * SIZE, .5 * SIZE, 2. * SIZE};
const Parts parts_arm =   { .4, .2 * SIZE, .5 * SIZE, .8 * SIZE};

// dynamics and collision objects
static dWorldID world;
static dBodyID id_body, id_left, id_right;
static dJointID id_hinge1, id_hinge2;

// state set by keyboard commands
static bool am_flag = false;

// èdêScmÇ…Ç®ÇØÇÈbdÇÃäpâ^ìÆó ÇãÅÇﬂÇƒdstÇ…ì¸ÇÍÇÈ
static void calc_am(dVector3 dst, dBodyID bd, dVector3 cm)
{
    const dReal *cm_bd =  dBodyGetPosition(bd);
    dVector3 r, orb;
    dMass m;
    unsigned i;

    /* é©ì]äpâ^ìÆó  */
    dBodyGetMass(bd, &m);
    dMassRotate(&m, dBodyGetRotation(bd));
    dMULTIPLYOP0_331(dst, =, m.I, dBodyGetAngularVel(bd));

    /* èdêSÇÃâÒÇËÇÃãOìπäpâ^ìÆó Çë´Ç∑ */
    for ( i = 0; i < 3; ++i ) r[i] = cm_bd[i] - cm[i];
    dCROSS(orb, =, r, dBodyGetLinearVel(bd));
    for ( i = 0; i < 3; ++i ) dst[i] += m.mass * orb[i];
}

// start simulation - set viewpoint

static void start()
{
    static float xyz[3] = {0.f,-2.3f,2.00f};
    static float hpr[3] = {90.0000f,-22.000f,0.0000f};
    dsSetViewpoint (xyz,hpr);
    printf ("Press 'l/L' to move left hand down/up.\n");
    printf ("Press 'r/R' to move right hand down/up.\n");
    printf ("Press 's' to slow down.\n");
    printf ("Press 'a' to toggle display of angular momentum.\n");
}

// called when a key pressed

static void command (int cmd)
{
    switch (cmd) {
    case 'a':
        am_flag = !am_flag;
        break;
    case 'l':
        dJointSetHingeParam (id_hinge1, dParamVel, +0.8);
        break;
    case 'L':
        dJointSetHingeParam (id_hinge1, dParamVel, -0.8);
        break;
    case 'r':
        dJointSetHingeParam (id_hinge2, dParamVel, +0.8);
        break;
    case 'R':
        dJointSetHingeParam (id_hinge2, dParamVel, -0.8);
        break;
    case 's':
        dBodySetAngularVel(id_body,0,0,0);
        dBodySetAngularVel(id_left,0,0,0);
        dBodySetAngularVel(id_right,0,0,0);
        break;
    default:
        ;
    }
}

// simulation loop

static void simLoop (int pause)
{
    const dReal *rot_body = dBodyGetRotation(id_body);
    const dReal *pos_body = dBodyGetPosition(id_body);
    const dReal *pos_left = dBodyGetPosition(id_left);
    const dReal *pos_right = dBodyGetPosition(id_right);
    dVector3 am, am_body, am_left, am_right;
    dVector3 cm; // ÇøÇ·ÇÒÇ∆èdêSÇÃà íuÇ‡åvéZÇµÇƒÇ‚ÇÈ
    unsigned i;

    if (!pause)
        dWorldStep (world,0.02);

    if ( am_flag ) {
        for ( i = 0; i < 3; ++i )
            cm[i] = ( pos_body[i] * parts_body.mass +
                      pos_left[i] * parts_arm.mass +
                      pos_right[i] * parts_arm.mass ) /
                    (parts_body.mass + 2 * parts_arm.mass);
        calc_am(am_body, id_body, cm);
        calc_am(am_left, id_left, cm);
        calc_am(am_right, id_right, cm);
        for ( i = 0; i < 3; ++i )
            am[i] = am_body[i] + am_left[i] + am_right[i];
    }

    if ( am_flag ) {
        dVector3 arrow;

        for ( i = 0; i < 3; ++i ) arrow[i] = cm[i] + 80 * am_body[i];
        dsSetColor(1,1,0); dsDrawLine(cm, arrow);
        for ( i = 0; i < 3; ++i ) arrow[i] = cm[i] + 80 * am_left[i];
        dsSetColor(1,0,0); dsDrawLine(cm, arrow);
        for ( i = 0; i < 3; ++i ) arrow[i] = cm[i] + 80 * am_right[i];
        dsSetColor(0,1,0); dsDrawLine(cm, arrow);
        for ( i = 0; i < 3; ++i ) arrow[i] = cm[i] + 80 * am[i];
        dsSetColor(1,1,1); dsDrawLine(cm, arrow);
    }

    dReal sides1[3] = {parts_body.width,parts_body.depth,parts_body.height};
    dReal sides2[3] = {parts_arm.width,parts_arm.depth,parts_arm.height};
    dVector3 pos_debeso;
    for ( i = 0; i < 3; ++i )
        pos_debeso[i] = pos_body[i] +
            parts_body.depth*.5 * dACCESS33(rot_body,i,1);

    dsSetTexture (DS_WOOD);
    dsSetColorAlpha(1,1,0, am_flag ? 0.5: 1);
    dsDrawBox (pos_body,rot_body,sides1);
    dsSetColorAlpha(0,0,0, am_flag ? 0.5: 1);
    dsDrawSphere(pos_debeso,rot_body,SIZE*.1);
    dsSetColorAlpha(1,0,0, am_flag ? 0.5: 1);
    dsDrawBox (pos_left,dBodyGetRotation(id_left),sides2);
    dsSetColorAlpha(0,1,0, am_flag ? 0.5: 1);
    dsDrawBox (pos_right,dBodyGetRotation(id_right),sides2);
}

int main (int argc, char **argv)
{
    // setup pointers to drawstuff callback functions
    dsFunctions fn;
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &simLoop;
    fn.command = &command;
    fn.stop = 0;
#ifdef _MSC_VER
    fn.path_to_textures = "./textures";
#else
    const char *textures = "/textures";
    const char *prog = getenv("_");
    char *dir = (char*) malloc(strlen(prog) + strlen(textures) + 1);
    strcpy(dir, prog);
    strcpy(strrchr(dir, '/'), textures);
    fn.path_to_textures = dir;
#endif

    // create world
    world = dWorldCreate();

    dMass m;

    id_body = dBodyCreate (world);
    dMassSetBox (&m,1,parts_body.width,parts_body.depth,parts_body.height);
    dMassAdjust (&m,parts_body.mass);
    dBodySetMass (id_body,&m);
    dBodySetPosition (id_body,0., 0., 1.);
    dBodySetAngularVel(id_body,0.2,0,0);

    id_left = dBodyCreate (world);
    dMassSetBox (&m,1,parts_arm.width,parts_arm.depth,parts_arm.height);
    dMassAdjust (&m,parts_arm.mass);
    dBodySetMass (id_left,&m);
    dBodySetPosition (id_left,-(parts_body.width+parts_arm.width)/2.,
                      0.,
                      1. + parts_body.height*.4 + parts_arm.height*.5);

    id_right = dBodyCreate (world);
    dMassSetBox (&m,1,parts_arm.width, parts_arm.depth, parts_arm.height);
    dMassAdjust (&m,parts_arm.mass);
    dBodySetMass (id_right,&m);
    dBodySetPosition (id_right,+(parts_body.width+parts_arm.width)/2.,
                      0.,
                      1. + parts_body.height*.4 + parts_arm.height*.5);

    id_hinge1 = dJointCreateHinge (world,0);
    dJointAttach (id_hinge1,id_body,id_left);
    dJointSetHingeAnchor (id_hinge1,0,0,1.+parts_body.height*.45);
    dJointSetHingeAxis (id_hinge1,1,0,0);
    dJointSetHingeParam (id_hinge1, dParamVel, 0.0);
    dJointSetHingeParam (id_hinge1, dParamFMax, 0.5);
    dJointSetHingeParam (id_hinge1, dParamHiStop, M_PI*.99);
    dJointSetHingeParam (id_hinge1, dParamLoStop, -M_PI*.01);

    id_hinge2 = dJointCreateHinge (world,0);
    dJointAttach (id_hinge2,id_body,id_right);
    dJointSetHingeAnchor (id_hinge2,0,0,1.+ parts_body.height * .45);
    dJointSetHingeAxis (id_hinge2,1,0,0);
    dJointSetHingeParam (id_hinge2, dParamVel, 0.0);
    dJointSetHingeParam (id_hinge2, dParamFMax, 0.5);
    dJointSetHingeParam (id_hinge2, dParamHiStop, M_PI*.99);
    dJointSetHingeParam (id_hinge2, dParamLoStop, -M_PI*.01);

    // run simulation
    dsSimulationLoop (argc,argv,352,288,&fn);

    dWorldDestroy (world);
    return 0;
}
