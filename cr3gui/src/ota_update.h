#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H


#ifndef FLASHDIR
    #ifndef DIRPREFIX
    #define DIRPREFIX ""
    #endif

    #define FLASHDIR DIRPREFIX "/mnt/ext1"
#endif


#define UNZIP_BINARY "/bin/unzip"
#define WGET_BINARY "/bin/wget"
#define MKDIR_BINARY "/bin/mkdir"
#define RM_BINARY "/bin/rm"
#define MV_BINARY "/bin/mv"

#define OTA_DOWNLOAD_DIR FLASHDIR
#define OTA_PACKAGE_NAME "latest.zip"
#define OTA_TEMP_DIR OTA_DOWNLOAD_DIR"/.cr3update"

// Sourceforge [old source]
// #define OTA_URL_BASE      "http://sourceforge.net/u/e379d14417/crengine/ci/pocketbook-port/tree/builds/"
// #define OTA_URL_POST      "?format=raw"
// #define OTA_URL_MASK      OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/" OTA_PACKAGE_NAME OTA_URL_POST
// #define OTA_URL_MASK_TEST OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/exists" OTA_URL_POST
// #define OTA_VERSION       OTA_URL_BASE "current.version" OTA_URL_POST
// #define OTA_LINK_MASK     OTA_URL_BASE "links/[DEVICE]" OTA_URL_POST


// Github [current source]
// #define OTA_URL_BIN_BASE  "http://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/builds/"
// #define OTA_URL_BASE      "https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/builds/"
// #define OTA_URL_MASK      OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/" OTA_PACKAGE_NAME
// #define OTA_URL_MASK_TEST OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/exists"
// #define OTA_VERSION       OTA_URL_BASE "current.version"
// #define OTA_LINK_MASK     OTA_URL_BASE "links/[DEVICE]"


// Github [current source]
#define OTA_URL_BASE      "http://brucelee.duckdns.org/cr3-pb-ota/"
#define OTA_URL_MASK      OTA_URL_BASE "?mode=download&dev=[DEVICE]&fw=" CR_PB_SDK
#define OTA_URL_MASK_TEST OTA_URL_BASE "?mode=exists&dev=[DEVICE]&fw=" CR_PB_SDK
#define OTA_VERSION       OTA_URL_BASE "?mode=version"
#define OTA_LINK_MASK     OTA_URL_BASE "?mode=link&dev=[DEVICE]"


#define OTA_VERSION_MAX_LENGTH 15
#define OTA_LINK_MAX_LENGTH    50
#define OTA_EXISTS_STR         "yes"


bool OTA_update();


#endif // OTA_UPDATE_H