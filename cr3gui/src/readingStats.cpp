
#include "readingStats.h"

time_t ReadingStats::getTime() {
    return (time_t) time(0);
}

lString8 ReadingStats::getDateTimeString() {
    return getDateTimeString(getTime());
}

lString8 ReadingStats::getDateTimeString(time_t t) {

    tm * bt = localtime(&t);
    char str[12];
    sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", bt->tm_year+1900, bt->tm_mon+1, bt->tm_mday, bt->tm_hour, bt->tm_min, bt->tm_sec);

    return lString8(str);
}

void ReadingStats::init() {
    minSeconds = 5;
    maxSeconds = 90;

    CRLog::trace("ReadingStats::init(): build path: \"%s\"", statsPath.c_str());
    iv_buildpath(statsPath.c_str());

    CRLog::trace("ReadingStats::init(): open config: \"%s\"", statsPath.c_str());
    iconfig *cfg = OpenConfig(statsPath.c_str(), NULL);

    CRLog::trace("ReadingStats::init()");
    // _lang = ReadString(cfg, const_cast<char *>("language"), const_cast<char *>("en"));
    // currentLang = lString16(_lang.c_str());
    // _keepOrientation = ReadInt(cfg, const_cast<char *>("keeporient"), 0);
    // _pbDictionary = ReadString(cfg, const_cast<char *>("dictionary"), const_cast<char *>(""));
    CloseConfig(cfg);
}

ReadingStats::ReadingStats(lString16 statsDir, lString16 statsFile) {

    statsPath = UnicodeToUtf8(statsDir + lString16("/") + statsFile);
    CRLog::trace("ReadingStats::statsPath = \"%s\"", statsPath.c_str());

    init();
}

void ReadingStats::markUserInteraction() {

}

void ReadingStats::breakUserInteraction() {

}

void ReadingStats::save() {

}
