#ifndef __CURLIT_HEADER_FILE__
#define __CURLIT_HEADER_FILE__
 
#include <map>
#include <string>
#include <iostream>
 
#include <curl/curl.h>

#define LIB_CURL_COOKIES_FILE "/mnt/ext1/system/config/cr3/cr3_cookies.txt"

using namespace std;
 
class web
{
protected:    // User declarations
    static char errorBuffer[CURL_ERROR_SIZE];
    static string buffer;
    static int writer(char *data, size_t size, size_t nmemb, string *buffer);
    static string easycurl(const string &url, bool post, const string &postparamstring);
    static string urlencode(const string &c);
    static string char2hex(char dec);    
 
public:        // User declarations
    web(){};
    ~web(){};
    static string post(const string &url, map<string, string> &abbrevs);
    static string get(const string &url, map<string, string> &abbrevs);
    static string get(const string &url);
 
};
#endif