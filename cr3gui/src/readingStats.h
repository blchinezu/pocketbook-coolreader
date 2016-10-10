
#ifdef POCKETBOOK_PRO

#ifndef __READING_STATS_H_INCLUDED__
#define __READING_STATS_H_INCLUDED__

#include "inkview.h"
#include "lvstring.h"

class ReadingStats {

private:
    int minSeconds;
    int maxSeconds;
    lString8 statsPath;

    // struct {
    //     int
    //     lString8 pbAction;
    // } stats;

    time_t getTime();

    lString8 getDateTimeString();
    lString8 getDateTimeString(time_t t);

    void init();

public:
    ReadingStats(lString16 statsDir, lString16 statsFile);
    void markUserInteraction();
    void breakUserInteraction();
    void save();
};

#endif

#endif
