#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <string>
using namespace std;

struct Sequence
{
    unsigned time;
    int command;
    string prefix;
    Sequence(unsigned t, int c, const string &s) : time(t), command(c), prefix(s) {}
};

struct Record
{
    string init_sequence;
    vector<Sequence> action_sequence;

    Record() { clear(); }

    void clear();
    void add_init(int c) { init_sequence += c; }
    void add_action(unsigned current_time, int cmd, const string &prefix = "");
    bool empty() { return action_sequence.empty(); }
    void do_save();
    bool do_load();
};

#endif
