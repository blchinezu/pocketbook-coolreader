#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H


#define PB_OTA_URL_BASE      "http://sourceforge.net/u/e379d14417/crengine/ci/pocketbook-port/tree/builds/"
#define PB_OTA_URL_MASK      PB_OTA_URL_BASE "[DEVICE]/"CR_PB_SDK"/latest.zip?format=raw"
#define PB_OTA_URL_MASK_TEST PB_OTA_URL_BASE "[DEVICE]/"CR_PB_SDK"/exists?format=raw"
#define PB_OTA_VERSION       PB_OTA_URL_BASE "current.version?format=raw"
#define PB_OTA_LINK_MASK     PB_OTA_URL_BASE "links/[DEVICE]?format=raw"
#define PB_OTA_VERSION_MAX_LENGTH 15
#define PB_OTA_LINK_MAX_LENGTH    50
#define PB_OTA_EXISTS_STR         "yes"


bool OTA_update();


#endif // OTA_UPDATE_H