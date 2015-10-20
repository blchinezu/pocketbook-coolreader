#include "wiki.h"

lString16 wiki::getWordDescription(const lString16 word) {
    lString16 url = lString16(WIKI_baseURL) + word;
    web::get();
}
