#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H


#ifndef TEMPDIR
    #ifndef DIRPREFIX
    #define DIRPREFIX ""
    #endif

    #define TEMPDIR DIRPREFIX "/tmp"
#endif


#define OTA_DOWNLOAD_DIR TEMPDIR
#define OTA_PACKAGE_NAME "latest.zip"


#define OTA_URL_BASE      "http://sourceforge.net/u/e379d14417/crengine/ci/pocketbook-port/tree/builds/"
#define OTA_URL_POST      "?format=raw"
#define OTA_URL_MASK      OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/" OTA_PACKAGE_NAME OTA_URL_POST
#define OTA_URL_MASK_TEST OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/exists" OTA_URL_POST
#define OTA_VERSION       OTA_URL_BASE "current.version" OTA_URL_POST
#define OTA_LINK_MASK     OTA_URL_BASE "links/[DEVICE]" OTA_URL_POST


#define OTA_VERSION_MAX_LENGTH 15
#define OTA_LINK_MAX_LENGTH    50
#define OTA_EXISTS_STR         "yes"


bool OTA_update();


#endif // OTA_UPDATE_H