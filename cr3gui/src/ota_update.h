/**
 * OTA Update implementation
 */

#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H


#ifndef FLASHDIR
    #ifndef DIRPREFIX
    #define DIRPREFIX ""
    #endif

    #define FLASHDIR DIRPREFIX "/mnt/ext1"
#endif


#define OTA_DOWNLOAD_DIR FLASHDIR
#define OTA_PACKAGE_NAME "latest.zip"
#define OTA_TEMP_DIR OTA_DOWNLOAD_DIR"/.cr3update"
#define OTA_INSTALL_SCRIPT OTA_TEMP_DIR"/install.sh"


// Sourceforge [old source]
// #define OTA_URL_BASE      "http://sourceforge.net/u/e379d14417/crengine/ci/pocketbook-port/tree/builds/"
// #define OTA_URL_POST      "?format=raw"
// #define OTA_URL_MASK      OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/" OTA_PACKAGE_NAME OTA_URL_POST
// #define OTA_URL_MASK_TEST OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/exists" OTA_URL_POST
// #define OTA_VERSION       OTA_URL_BASE "current.version" OTA_URL_POST
// #define OTA_LINK_MASK     OTA_URL_BASE "links/[DEVICE]" OTA_URL_POST


// Github [current source]
#define OTA_BRANCH_STABLE "master"
#define OTA_BRANCH_DEV    "dev"
#define OTA_URL_BASE      "https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/[BRANCH]/builds/"
#define OTA_URL_MASK      OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/" OTA_PACKAGE_NAME
#define OTA_URL_MASK_TEST OTA_URL_BASE "[DEVICE]/" CR_PB_SDK "/exists"
#define OTA_VERSION       OTA_URL_BASE "current.version"
#define OTA_LINK_MASK     OTA_URL_BASE "links/[DEVICE]"

#define OTA_DEV_MARKER    FLASHDIR"/system/.cr3-use-devel-branch"


#define OTA_VERSION_MAX_LENGTH 15
#define OTA_LINK_MAX_LENGTH    50
#define OTA_EXISTS_STR         "yes"


bool OTA_update(const char * branch);


#endif // OTA_UPDATE_H
