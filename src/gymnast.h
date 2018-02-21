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

    /* �e��̊�{�����ݒ�B
       �L�[�{�[�h����Akey�̃L�[�����������ɔ��������B
       key�������O�ɁA�����̗������Ɠ���̃o���G�[�V����(prefix)�̎w�肪�ł���B
       ���݂��Ȃ��o���G�[�V�������w�肵���Ƃ��́A��{�̓���(prefix�Ȃ�)�ɂȂ�B

       fmax�ȓ��̃g���N���g���āA�Ԑڂ�vel�̑��x�œ������āA�w�肵���p�x�ɂ��悤�Ƃ���B
       �p�x�� degree�Ŏw��B�g���N�́A[�̏d * g * 1m]�� 1�Ƃ��Ďw��B
       �w�肷��p�x��
         -dInfinity�ɂ����Ƃ��́A���̊֐߂Ɋւ��ẮA���̓����ۂB
         +dInfinity�ɂ����Ƃ��́A���ɂǂ̊p�x�܂ŁA�Ƃ����ڕW�Ȃ��ɁA���̓����
           �w�肳���܂ŗ͂�����������B

       vel�� 0�ɂ����֐߂́Afmax�̈ȓ��̗͂��g���āA���̊֐߂𓮂����Ȃ��悤�ɂ���B
         �܂�A���̊֐߂���߂�B
       fmax = 0�ɂ����Ƃ��ɂ́A���̊֐߂͒E�͂���B
    */
    void install_movement(
        char key,
        string prefix,
        const string &message,

        dReal am, dReal am_vel, dReal am_fmax,     // ��̊p�x
        dReal mu, dReal mu_vel, dReal mu_fmax,     // ���p�x
        dReal hm, dReal hm_vel, dReal hm_fmax,     // ���̊J��
        dReal kh, dReal kh_vel, dReal kh_fmax,     // ���̔���
        dReal l1k, dReal l1k_vel, dReal l1k_fmax,  // ���̊p�x
        dReal l21, dReal l21_vel, dReal l21_fmax   // �G�p�x
        );

    void set_movement(char key, string prefix);

    void move();
    void plus();
    void minus();
    void print_message();
    /* �f�o�b�O�p */
    void dump_state();
};

#endif
