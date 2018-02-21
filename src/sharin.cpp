/* 車輪シミュレータ ver 0.4
   by たむぶん
   http://club.pep.ne.jp/~tamubun/
   http://bunysmc.exblog.jp/
*/

#include <ode/ode.h>
#include <drawstuff/drawstuff.h>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#include "addition.h"
#include "object.h"
#include "gymnast.h"
#include "record.h"
#include "mode.h"

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#else
#include <stdlib.h>
#include <string.h>
#endif

#define HIGH (2.55) // 鉄棒の高さ
#define MAX_CONTACTS (4)

Bar *bar;
Gymnast *gymnast = 0;
Record record;

static bool release_flag = false; // here: 書き方良くない

static dReal stepsize = 0.05;
static unsigned current_time;
static unsigned sequence_num;

/* 現在のモード */
static Mode *mode = Mode::setup_mode;

/* パラメータ調整用 */
static dReal aaa = 0;

static void movement_init()
{
    gymnast->install_movement(  // 完全脱力
        'r', "",
        "Kanzen ni datsuryoku.",
        0            ,  0,   0,
        0            ,  0,   0,
        0            ,  0,   0,
        0            ,  0,   0,
        0            ,  0,   0,
        0            ,  0,   0);

#if 0
    gymnast->install_movement(  // 実験
        'e', "",
        "Jikken. Variation=(1,2)",
         -10.,0.023, 0.03,
           0,     0,   0,
         +12.,0.023, 0.16,
         +12.,0.023, 0.18,
        +103.,0.043, 0.21,
           0.,0.013, 0.05);

    gymnast->install_movement(  // 実験(1)
        'e', "1",
        "",
         -10.,0.023, 0.03,
        -110, 0.033, 0.21,
         +30.,0.033, 0.21,
         +35.,0.033, 0.21,
        +110.,0.033, 0.21,
           0.,0.013, 0.05);

    gymnast->install_movement(  // 実験(2)
        'e', "2",
        "",
         -10.,0.063, 0.03,
          -5, 0.083, 0.55,
         +10.,0.063, 0.41,
         +10.,0.063, 0.31,
          +5.,0.063, 0.21,
           0.,0.063, 0.05);
#endif

    gymnast->install_movement(  // 押し。白樺
        'z', "",
        "Oshi. Variation = (1,2)",
         +25., 0.3, 0.02,
        -145., 0.4, 0.115,
          +3., 0.3, 0.115,
          +3., 0.3, 0.10,
          +5., 0.5, 0.08,
          +0., 0.2, 0.10);
    gymnast->install_movement(  // 押しバリエーション。腰を取る
        'z', "1",
        "",
         +25., 1.0, 0.02,
        -145., 1.0, 0.115,
          +3., 1.0, 0.115,
          +3., 1.0, 0.115,
         +35., 1.0, 0.08,
          +0., 1.0, 0.10);
    gymnast->install_movement(  // 押しバリエーション。胸が落ちる
        'z', "2",
        "",
          +25., 1.0, 0.02,
         -145., 1.0, 0.115,
          -10., 1.0, 0.115,
          -15., 1.0, 0.115,
          +40., 1.0, 0.08,
           +0., 1.0, 0.10);

    gymnast->install_movement(  // 抜き。意識的に反る
        'q', "",
        "Nuki. Variation = (1,2)",
          -5., 0.8, 0.02,
        -160., 0.8, 0.02,
          -5., 0.8, 0.04,
          -5., 0.8, 0.04,
         -25., 0.8, 0.04,
          +0., 0.8, 0.10);
    gymnast->install_movement(  // 抜きバリエーション。首と膝以外は脱力
        'q', "1",
        "",
          -5., 0.8, 0.02,
           0.,   0,   0,
           0.,   0,   0,
           0.,   0,   0,
           0.,   0,   0,
          +0., 0.8, 0.10);
    gymnast->install_movement(  // 抜きバリエーション。肩に力入りっぱなし
        'q', "2",
        "",
          -5., 0.8, 0.02,
        -120., 0.8, 0.08,
          +5., 0.8, 0.04,
           0., 0.8, 0.04,
         -25., 0.8, 0.04,
          +0., 0.8, 0.10);

    gymnast->install_movement(  // 胸当て
        's', "",
        "Mune ate.",
         +25., 1.0, 0.02,
        -155., 2.5, 0.05,
         -15., 2.5, 0.04,
         -15., 2.5, 0.03,
         -25., 2.5, 0.03,
          +0., 1.0, 0.03);

    gymnast->install_movement(  // あふり
        'a', "",
        "Ahuri.",
          -5., 0.1, 0.03,
        -140., 0.1, 0.01,
         +20., 0.1, 0.14,
         +20., 0.1, 0.14,
         +dInfinity, 0.87, 0.17,
          +0., 0.1, 0.15);

    gymnast->install_movement(  // 逆車の肩
        'x', "",
        "Kata dashi.",
         +30., 0.1, 0.03,
        -110., 0.2, 0.14,
         +15., 0.1, 0.14,
         +15., 0.1, 0.15,
         +25., 0.2, 0.15,
          +0., 0.1, 0.05);

    gymnast->install_movement(  // 蹴上り
        't', "",
        "keagari. hikiyose",
         -20., 0.2, 0.02,
        -135., 0.5, 0.26,
         +35., 0.3, 0.28,
         +45., 0.3, 0.28,
        +120., 0.8, 0.28,
          +0., 0.1, 0.8);

    gymnast->install_movement(  // 蹴上り後半
        'y', "",
        "keagari keri.",
         -10., 0.5, 0.08,
          +5., 0.6, 0.28,
         +15., 1.0, 0.28,
         +15., 1.0, 0.28,
         +40., 1.0, 0.28,
          +0., 0.5, 0.15);

    gymnast->install_movement(  // 膝曲げ
        'h', "",
        "Hiza mage. Variation = (1,2)",
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
         +60., 1.0, 0.1,
        -130., 1.0, 0.1);
    gymnast->install_movement(  // 膝曲げのバリエーション。膝しか曲げない
        'h', "1",
        "",
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
          +0., 1.0, 0.1,
        -130., 1.0, 0.1);
    gymnast->install_movement(  // 膝曲げのバリエーション。膝を伸ばす
        'h', "2",
        "",
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
         +10., 1.0, 0.1,
           0., 1.0, 0.1);

    gymnast->install_movement(  // 首起こす
        'k', "",
        "Kubi mage. Variation = (1,2)",
         +25., 1.0, 0.05,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0);
    gymnast->install_movement(  // 首バリエーション。入れる
        'k', "1",
        "",
         -25., 1.0, 0.05,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0);
    gymnast->install_movement(  // 首バリエーション。まっすぐに
        'k', "2",
        "",
         +0., 1.0, 0.05,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0);

    gymnast->install_movement(  // 腕を下げる
        'u', "",
        "Ude wo sageru. Variation = (1,2)",
        -dInfinity, 0,0,
         +20,  0.7, 0.1,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0);
    gymnast->install_movement(  // 腕バリエーション。上げる
        'u', "1",
        "",
        -dInfinity, 0,0,
         -150,  0.7, 0.1,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0,
        -dInfinity, 0,0);
}

static void objects_init()
{
    Object::world_init();

    bar = new Bar(1,1,1, 10, .80, .024, HIGH, 150, 120, .004);
    dBodyDisable(bar->id);

    if ( gymnast != 0 )
        delete gymnast;
    gymnast = new Gymnast(HIGH);
    mode->set_gymnast(gymnast);
    movement_init();
    release_flag = false;
    current_time = 0;
}

static void start()
{
    static float xyz[3] = {5.3, 0.0, 2.86};
    static float hpr[3] = {174, -11.5, 0};
    dsSetViewpoint (xyz,hpr);

    objects_init();

    current_time = 0;
    mode->enter();
}

static void command (int cmd)
{
    unsigned i, j;
    static string prefix = "";
    static dReal rad = 0;
    const dReal d_rad = RAD(29.9);
    const dReal d_rad_f = RAD(14.9);
    dWorldID &world = Object::world;

    switch ( mode->type ) {
    case Mode::SETUP:
        switch ( cmd ) {
        case ' ':
            dBodyEnable(bar->id);
            gymnast->arm->connect(bar, 0, HIGH, -1, -1);
            dJointSetFeedback(gymnast->arm->hinge, bar->feedback);
            gymnast->set_movement('z', "");
            dWorldSetGravity(world, 0, 0, -GRAVITY);
            mode = mode->transition(cmd);   // Simulaton Mode
            break;
        case 'V':
            objects_init();
            mode = mode->transition(cmd);   // Video Mode
            break;
        case 'a':
        case 's':
        case 'q':
        case 'w':
            record.add_init(cmd);
            if ( cmd == 'a' )
                rad -= d_rad;
            else if ( cmd == 'q' )
                rad -= d_rad_f;
            else if ( cmd == 's' )
                rad += d_rad;
            else
                rad += d_rad_f;
            dWorldSetGravity(world, 0, 0.02 * sin(rad), -0.02 * cos(rad));
            for ( i = 0; i < 1000; ++i ) {
                dWorldStep(world, stepsize);
                if ( i % 100 == 0 )
                    gymnast->stop(true);
            }
            gymnast->stop();
            break;
        default:
            ;
        }
        break;

    case Mode::VIDEO:
        switch ( cmd ) {
        case 'R':
            objects_init();
            record.clear();
            prefix = "";
            rad = 0;
            mode = mode->transition(cmd);   // Setup Mode
            break;
        case 'S':
            record.do_save();
            break;
        case 'L':
            if (! record.do_load() )
                break;
            /* else go thru */
        case ' ':
            objects_init();
            rad = 0;
            for ( j = 0; j < record.init_sequence.size(); ++j ) {
                switch ( record.init_sequence[j] ) {
                case 'a':
                    rad -= d_rad;
                    break;
                case 'q':
                    rad -= d_rad_f;
                    break;
                case 's':
                    rad += d_rad;
                    break;
                case 'w':
                    rad += d_rad_f;
                }
                dWorldSetGravity(world, 0, 0.02 * sin(rad), -0.02 * cos(rad));
                for ( i = 0; i < 1000; ++i ) {
                    dWorldStep(world, stepsize);
                    if ( i % 100 == 0 )
                        gymnast->stop(true);
                }
                gymnast->stop();
            }
            dBodyEnable(bar->id);
            gymnast->arm->connect(bar, 0, HIGH, -1, -1);
            dJointSetFeedback(gymnast->arm->hinge, bar->feedback);
            gymnast->set_movement('z', "");
            dWorldSetGravity(world, 0, 0, -GRAVITY);
            sequence_num = 0;
            aSetPause(0);

            break;
        default:
            ;
        }
        break;

    default:
        switch ( cmd ) {
        case 'R':
            objects_init();
            record.clear();
            prefix = "";
            rad = 0;
            mode = mode->transition(cmd);   // Setup Mode
            break;
        case 'V':
            record.add_action(current_time, '0');
            objects_init();
            rad = 0;
            mode = mode->transition(cmd);   // Video Mode
            break;
        case 'o':
            record.add_action(current_time, cmd);
            release_flag = true;
            break;
        case ' ':
            aSetPause(1-aGetPause());
            break;
        case '+':
            record.add_action(current_time, cmd);
            gymnast->plus();
            break;
        case '-':
            record.add_action(current_time, cmd);
            gymnast->minus();
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            prefix += cmd;
            break;
        default:
            record.add_action(current_time, cmd, prefix);
            gymnast->set_movement(cmd, prefix);
            prefix = "";
        }
    }
}

static void auto_command()
{
    while ( current_time == record.action_sequence[sequence_num].time ) {
        const Sequence &s = record.action_sequence[sequence_num++];
        switch ( s.command ) {
        case '0':
            aSetPause(1);
            break;
        case 'o':
            release_flag = true;
            break;
        case '+':
            gymnast->plus();
            break;
        case '-':
            gymnast->minus();
            break;
        default:
            gymnast->set_movement(s.command, s.prefix);
        }
    }
}

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    unsigned i;
    unsigned numc;
    dContact contact[MAX_CONTACTS];
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if ( b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact) )
        return;

    for ( i = 0; i < MAX_CONTACTS; ++i ) {
        dSurfaceParameters &surface = contact[i].surface;
        surface.mode = dContactSoftERP | dContactSoftCFM;
        surface.mu = 0;
        surface.soft_erp = 0.8;
        surface.soft_cfm = 0.028; //この値を大きくするとバーを通り抜け易くなる
    }

    numc = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
    if ( numc ) {
        for ( i = 0; i < numc; ++i) {
            dJointID c = dJointCreateContact(Object::world, gymnast->jgid, contact + i);
            dJointAttach(c, b1, b2);
        }
    }
}

static void simLoop (int pause)
{
    if ( !pause ) {
        if ( mode->type == Mode::VIDEO && !record.empty() )
            auto_command();

        dSpaceCollide2((dxGeom*) gymnast->space, bar->gid, 0, &nearCallback);

        if ( release_flag && dJointGetBody(gymnast->arm->hinge, 0) != 0 ) {
            dJointAttach(gymnast->arm->hinge, 0, 0);
        }

        dVector3 spring, friction;
        bar->getForce(spring);          // しなり
        dBodyAddForce(bar->id, spring[0], spring[1], spring[2]);
        bar->getTorque(friction, gymnast->arm);  // 摩擦
        dBodyAddTorque(gymnast->arm->id, friction[0], friction[1], friction[2]);
        dBodySetTorque(bar->id, 0, 0, 0);
        dBodySetAngularVel(bar->id, 0, 0, 0);
        const dReal *tmp = dBodyGetLinearVel(bar->id);
        dBodySetLinearVel(bar->id, 0, tmp[1], tmp[2]);
        gymnast->move();
        dWorldStep(Object::world, stepsize);

        dJointGroupEmpty(gymnast->jgid);
        ++current_time;
    }

    bar->draw();
    gymnast->draw();

    if ( mode->type != Mode::SIMULATION ) {
        glPushAttrib (GL_TRANSFORM_BIT |  /* for matrix contents */
                      GL_ENABLE_BIT);     /* for various glDisable calls */
        glDisable (GL_LIGHTING);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_TEXTURE_2D);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glPointSize(15.f);
        if ( mode->type == Mode::SETUP )
            glColor3f(0.0f, 0.0f, 0.0f);
        else
            glColor3f(0.0f, 0.8f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(-0.9f, 0.9f);
        glEnd();
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glPopAttrib();
    }
}

int main (int argc, char **argv)
{
    if (argc > 2 && strcmp(argv[1],"-aaa")==0) {
        char *endptr;
        aaa = strtod(argv[2],&endptr);
    }

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

    // run simulation
    dsSimulationLoop (argc,argv,352,288,&fn);

    return 0;
}
