#include <cstring>
#include <cstdio>
#include <cctype>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

#include "record.h"

#define BUF_SIZE (256)
#define LINE "%254[^\f\n\r\t\v]" // " "は含まない

void
save(const Record &record)
{
    int fd;
    string filename, yn;

  again:
    cout << "Save File Name: ";
    cin >> filename;
    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0644);
    if ( fd == -1 && errno == EEXIST ) {
        cout << "File: " << filename << " exists. Rewrite? (Y/N) ";
        cin >> yn;
        if ( yn == "Y" || yn == "y" ) {
            fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        } else {
            goto again;
        }
    }

    if ( fd == -1 ) {
        cerr << "Save failed." << endl;
        return;
    }

    FILE *file = fdopen(fd, "w");

    // バージョン番号
    fprintf(file, "%s\n", "1");

    /* 初期配置 */
    if ( record.init_sequence.length() >= BUF_SIZE-1 ) {
        cout << "Too long init sequence.\nSave Failed." << endl;
        return;
    }
    fprintf(file, "%s\n", record.init_sequence.c_str());

    /* 動作 */
    for ( unsigned i = 0; i < record.action_sequence.size(); ++i ) {
        const Sequence &s = record.action_sequence[i];
        if ( s.prefix.length() > 10 ) {
            cout << "Too long prefix.\nSave Failed." << endl;
            return;
        }
        fprintf(file, "%d %c %s", s.time, s.command, s.prefix.c_str());
        fprintf(file, "\n");
    }

    fclose(file);
    cout << "Save done." << endl;
}

static const char *
readln(FILE *file)
{
    static char buf[BUF_SIZE];
    int c;

    bzero(buf, BUF_SIZE);
    int n = fscanf(file, LINE, buf);
    if ( n != 1 || strlen(buf) >= BUF_SIZE - 1 )
        return 0;
    while ( isspace(c = fgetc(file)) )
        ;
    ungetc(c, file);

    return buf;
}

bool
load(Record &record)
{
    int fd;
    string filename;

    printf("Load File Name: ");
    cin >> filename;
    fd = open(filename.c_str(), O_RDONLY);
    if ( fd == -1 ) {
        cout << "File: " << filename << " open failed." << endl;
        return false;
    }
    FILE *file = fdopen(fd, "r");

    int n;
    const char *line;
    int var;
    int t;
    char com;
    char opt[BUF_SIZE];
    vector<Sequence> sequence;
    string init_sequence;

    /* バージョン番号 */
    line = readln(file);
    if ( line == 0 )
        goto error;
    n = sscanf(line, "%d", &var);
    if ( n != 1 || var != 1 )
        goto error;

    /* 初期配置 */
    line = readln(file);
    if ( line == 0 )
        goto error;
    init_sequence = line;

    /* 動作 */
    do {
        line = readln(file);
        if ( line == 0 )
            goto error;

        bzero(opt, BUF_SIZE);
        n = sscanf(line, "%d %c %10[0-9]", &t, &com, opt);
        if ( n < 2 || n > 3 )
            goto error;
        sequence.push_back(Sequence(t, com, string(opt)));
    } while ( com != '0' );

    record.init_sequence = init_sequence;
    record.action_sequence = sequence;

    return true;

  error:
    cout << "File format error" << endl;
    return false;
}
