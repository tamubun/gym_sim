#ifndef FILES_H
#define FILES_H

class Record;

extern void save(const Record &record);
extern bool load(Record &record);

#endif
