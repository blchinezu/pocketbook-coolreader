#include <crengine.h>
#include "web.h"
 
//have to re-declare static members at file level
char web::errorBuffer[CURL_ERROR_SIZE];
string web::buffer;
 
//---------------------------------------------------------------------------
string web::get(const string &url)
{
    bool   usepost=false;
    string params ="";
    return easycurl(url, usepost, params);
}
//---------------------------------------------------------------------------
bool web::getBinary(const string &url, const string &outfilename)
{
    CRLog::trace("web::getBinary(%s, %s)", url.c_str(), outfilename.c_str());

    bool   usepost=false;
    string params ="";
    return easycurlBinary(url, usepost, params, outfilename);
}
//---------------------------------------------------------------------------
string web::get(const string &url, map<string, string> &m)
{
    bool usepost=false;
    string poststring="";
 
    map<string, string>::iterator curr,end;
    for(curr = m.begin(), end = m.end(); curr != end; curr++)
    {
        poststring+= curr->first + "=" + web::urlencode(curr->second)+ "&";
    }
 
    return easycurl(url, usepost, poststring);
}
//---------------------------------------------------------------------------
string web::post(const string &url, map<string, string> &m)
{
    bool usepost=true;
    string poststring="";
 
    map<string, string>::iterator curr,end;
    for(curr = m.begin(), end = m.end(); curr != end; curr++)
    {
        poststring+= curr->first + "=" + web::urlencode(curr->second)+ "&";
    }
 
    return easycurl(url, usepost, poststring);
}
//---------------------------------------------------------------------------
string web::easycurl(const string &url, bool post, const string &postparamstring)
{
    CRLog::trace("web::easycurl(%s)", url.c_str());
 
    // Our curl objects
    buffer="";
    errorBuffer[0]=0;
 
    CURL *curl;
    CURLcode result;
 
    // Create curl handle
    CRLog::trace("web::easycurl(): Create curl handle");
    curl = curl_easy_init();
 
    if (curl)
    {
      // Now set up all of the curl options
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
      if (post)
      {
          curl_easy_setopt(curl, CURLOPT_POST,1);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS,postparamstring.c_str());
      }
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,  2);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)");
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // this line makes it work under https
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, LIB_CURL_COOKIES_FILE);//read from
      curl_easy_setopt(curl, CURLOPT_COOKIEJAR, LIB_CURL_COOKIES_FILE); //write to
 
      // Attempt to retrieve the remote page
      CRLog::trace("web::easycurl(): Attempt to retrieve the remote page");
      result = curl_easy_perform(curl);
 
      // Always cleanup
      curl_easy_cleanup(curl);
 
      // Did we succeed?
      if (result == CURLE_OK)
      {
          CRLog::trace("web::easycurl(): Got page");
          return buffer;
      }
      else
      {
          CRLog::trace("web::easycurl(): Couldn't get page");
          cerr << "error:" << errorBuffer <<endl;
          return "";
      }
    }
 
    return "";
}
//---------------------------------------------------------------------------
bool web::easycurlBinary(const string &url, bool post, const string &postparamstring, const string &outfilename)
{
    CRLog::trace("web::easycurlBinary(%s, bool, %s, %s)", url.c_str(), postparamstring.c_str(), outfilename.c_str());
 
    // Our curl objects
    errorBuffer[0]=0;
 
    FILE *fp;
    CURL *curl;
    CURLcode result;
 
    // Create curl handle
    CRLog::trace("web::easycurlBinary(): Create curl handle");
    curl = curl_easy_init();
 
    if (curl)
    {
      // Open file for writing
      CRLog::trace("web::easycurlBinary(): Open file for writing");
      fp = fopen(outfilename.c_str(),"wb");

      // Now set up all of the curl options
      CRLog::trace("web::easycurlBinary(): Now set up all of the curl options");
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
      curl_easy_setopt(curl, CURLOPT_HEADER, 0);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writerBinary);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
      if (post)
      {
          curl_easy_setopt(curl, CURLOPT_POST,1);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS,postparamstring.c_str());
      }
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,  2);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)");
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // this line makes it work under https
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, LIB_CURL_COOKIES_FILE);//read from
      curl_easy_setopt(curl, CURLOPT_COOKIEJAR, LIB_CURL_COOKIES_FILE); //write to
 
      // Attempt to retrieve the remote page
      CRLog::trace("web::easycurlBinary(): Attempt to retrieve the remote binary");
      result = curl_easy_perform(curl);
 
      // Always cleanup
      CRLog::trace("web::easycurlBinary(): Cleanup");
      curl_easy_cleanup(curl);
 
      // Close file handle
      CRLog::trace("web::easycurlBinary(): Close file handle");
      fclose(fp);

      // Did we succeed?
      if (result == CURLE_OK)
      {
          CRLog::trace("web::easycurlBinary(): Got page");
          return true;
      }
      else
      {
          CRLog::trace("web::easycurlBinary(): Couldn't get page");
          cerr << "error:" << errorBuffer <<endl;
          return false;
      }
    }
 
    CRLog::trace("web::easycurlBinary(): Couldn't initialize curl");
    return false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int web::writer(char *data, size_t size, size_t nmemb, string *buffer)
{
  int result = 0;
  if (buffer != NULL)
  {
    buffer->append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
}
//---------------------------------------------------------------------------
size_t web::writerBinary(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
//---------------------------------------------------------------------------
string web::urlencode(const string &c)
{
 
    string escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'') //~!*()'
        )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}
//-----------------------------------------------------------------------------
string web::char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;
 
    string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}
//-----------------------------------------------------------------------------
