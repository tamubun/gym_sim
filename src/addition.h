#ifndef _ADDITION_H

#include <ode/common.h>

struct dMass;

void
aMassSetEllipsoid(dMass *m, dReal density, dReal lx, dReal ly, dReal lz);
void
aMassSetEllipsoidTotal(dMass *m, dReal total_mass, dReal lx, dReal ly, dReal lz);

void
aDrawEllipsoid(const float pos[3], const float R[12], const float sides[3]);

void
aDrawEllipsoidD(const double pos[3], const double R[12], const double sides[3]);

int
aGetPause();

void
aSetPause(int p);

#endif
