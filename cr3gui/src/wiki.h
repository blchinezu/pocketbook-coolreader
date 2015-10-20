#ifndef __WIKI_HEADER_FILE__
#define __WIKI_HEADER_FILE__
 
#include <web.h>

#define WIKI_baseURL "https://en.wikipedia.org/w/api.php?action=query&prop=revisions&rvprop=content&format=xml&titles="

extern lString16 appLanguage;

class wiki
{
protected:

public:
    wiki(){};
    ~wiki(){};
    
    static lString16 getWordDescription(const lString16 * word);
};

#endif
