#ifndef GYMNAST_H
#define GYMNAST_H

#include <ode/ode.h>
#include <vector>
#include <string>
using namespace std;

struct Object;
struct Body;
struct Leg;
struct Movement;

struct Gymnast
{
    Body *atama;
    Body *mune;
    Body *hara;
    Body *koshi;
    Leg *arm;
    Leg *leg1;
    Leg *leg2;

    dJointGroupID jgid;
    dSpaceID space;

    vector<Object*> parts;
    dReal weight;

    Movement *cur_movement;

    Gymnast(dReal heigh);
    ~Gymnast();

    void draw();

    void stop(bool linear_only = false);

    /* 各種の基本動作を設定。
       キーボードから、keyのキーを押した時に発動される。
       keyを押す前に、数字の列を入れると動作のバリエーション(prefix)の指定ができる。
       存在しないバリエーションを指定したときは、基本の動作(prefixなし)になる。

       fmax以内のトルクを使って、間接をvelの速度で動かして、指定した角度にしようとする。
       角度は degreeで指定。トルクは、[体重 * g * 1m]を 1として指定。
       指定する角度を
         -dInfinityにしたときは、その関節に関しては、元の動作を保つ。
         +dInfinityにしたときは、特にどの角度まで、という目標なしに、他の動作を
           指定されるまで力を加え続ける。

       velを 0にした関節は、fmaxの以内の力を使って、その関節を動かさないようにする。
         つまり、その関節を締める。
       fmax = 0にしたときには、その関節は脱力する。
    */
    void install_movement(
        char key,
        string prefix,
        const string &message,

        dReal am, dReal am_vel, dReal am_fmax,     // 首の角度
        dReal mu, dReal mu_vel, dReal mu_fmax,     // 肩角度
        dReal hm, dReal hm_vel, dReal hm_fmax,     // 胸の開き
        dReal kh, dReal kh_vel, dReal kh_fmax,     // 腰の反り
        dReal l1k, dReal l1k_vel, dReal l1k_fmax,  // 足の角度
        dReal l21, dReal l21_vel, dReal l21_fmax   // 膝角度
        );

    void set_movement(char key, string prefix);

    void move();
    void plus();
    void minus();
    void print_message();
    /* デバッグ用 */
    void dump_state();
};

#endif
