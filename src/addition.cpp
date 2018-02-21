/* ODEの補足ルーチン。楕円体用の関数を追加。
   ODEのソースから必要な部分はコピーしている

   by たむぶん
   http://club.pep.ne.jp/~tamubun/
   http://bunysmc.exblog.jp/
*/

#ifdef WIN32
#include <windows.h>
#endif
#include <ode/config.h>
#include <ode/mass.h>
#include <ode/odemath.h>
#include <ode/matrix.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "addition.h"

#define _I(i,j) I[(i)*4+(j)]

static int checkMass (dMass *m)
{
    int i;

    if (m->mass <= 0) {
        dDEBUGMSG ("mass must be > 0");
        return 0;
    }
    if (!dIsPositiveDefinite (m->I,3)) {
        dDEBUGMSG ("inertia must be positive definite");
        return 0;
    }

    dMatrix3 I2,chat;
    dSetZero (chat,12);
    dCROSSMAT (chat,m->c,4,+,-);
    dMULTIPLY0_333 (I2,chat,chat);
    for (i=0; i<3; i++) I2[i] = m->I[i] + m->mass*I2[i];
    for (i=4; i<7; i++) I2[i] = m->I[i] + m->mass*I2[i];
    for (i=8; i<11; i++) I2[i] = m->I[i] + m->mass*I2[i];
    if (!dIsPositiveDefinite (I2,3)) {
        dDEBUGMSG ("center of mass inconsistent with mass parameters");
        return 0;
    }
    return 1;
}

void
aMassSetEllipsoid(dMass *m, dReal density, dReal lx, dReal ly, dReal lz)
{
    aMassSetEllipsoidTotal(m, (REAL(1.0)/REAL(6.0)) * M_PI * lx * ly * lz * density,
                           lx, ly, lz);
}

void
aMassSetEllipsoidTotal(dMass *m, dReal total_mass, dReal lx, dReal ly, dReal lz)
{
    dAASSERT (m);
    dMassSetZero (m);
    m->mass = total_mass;
    dReal II = REAL(0.4) * total_mass/ REAL(4.0);
    m->_I(0,0) = II * ly * lz;
    m->_I(1,1) = II * lz * lx;
    m->_I(2,2) = II * lx * ly;

# ifndef dNODEBUG
    checkMass (m);
# endif
}
