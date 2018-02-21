using namespace std;

#include "files.h"
#include "record.h"

#ifdef _MSC_VER
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

void
Record::clear()
{
    init_sequence = "";
    action_sequence.clear();
}

void
Record::add_action(unsigned current_time, int cmd, const string &prefix)
{
    action_sequence.push_back(Sequence(current_time, cmd, prefix));
}

void
Record::do_save()
{
    save(*this);
}

bool
Record::do_load()
{
    clear();
    return load(*this);
}
