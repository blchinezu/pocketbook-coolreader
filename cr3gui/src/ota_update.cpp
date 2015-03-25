
#include <inkview.h>
#include <inkplatform.h>
#include <crengine.h>
#include <crgui.h>
#include <cri18n.h>
#include "web.h"
#include "cr3pocketbook.h"
#include "ota_update.h"


/**
 * Check if there is a new version
 *
 * @return  true if new version, false otherwise
 */
bool OTA_isNewVersion() {
    if( !pbNetwork("connect") )
        return false;
    const char * response = web::get(PB_OTA_VERSION).c_str();
    return
        strlen(response) > 5 &&
        strlen(response) <= PB_OTA_VERSION_MAX_LENGTH &&
        strcmp(CR_PB_VERSION, response) != 0;
}
/**
 * Checks if the test link is fine
 *
 * @param  url  url
 *
 * @return  true if ok, false if not
 */
bool OTA_downloadExists(const lString16 url) {
    if( !pbNetwork("connect") )
        return false;
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    return
        strlen(response) == strlen(PB_OTA_EXISTS_STR) &&
        strcmp(PB_OTA_EXISTS_STR, response) == 0;
}

/**
 * Gets a device model if the current device is linked
 *
 * @param  deviceModel  current device model number
 *
 * @return  linked device model or empty if not linked
 */
lString16 OTA_getLink(const lString16 deviceModel) {
    if( !pbNetwork("connect") )
        return NULL;
    lString16 url = lString16(PB_OTA_LINK_MASK);
    url.replace(lString16("[DEVICE]"), deviceModel);
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    if( strlen(response) > 0 && strlen(response) <= PB_OTA_VERSION_MAX_LENGTH )
        return lString16(response);
    return lString16("");
}

/**
 * Generate url from mask and device model number
 *
 * @param  mask         url mask
 * @param  deviceModel  device model number
 *
 * @return  url
 */
lString16 OTA_genUrl(const char *mask, const lString16 deviceModel) {
    lString16 url = lString16(mask);
    url.replace(lString16("[DEVICE]"), deviceModel);
    return url;
}

/**
 * This does the actual updating of the app
 *
 * @param  url  url
 *
 * @return  true if updated, false if error
 */
bool OTA_updateFrom(const lString16 url) {

    Message(ICON_INFORMATION,  const_cast<char*>("CoolReader"),
        UnicodeToUtf8(url).c_str(), 5000);

    return false;
}

/**
 * Main func to be called for updating
 *
 * @return  true if updated, false if not
 */
bool OTA_update() {

    // Network Connect
    if( !pbNetwork("connect") ) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to the network!"), 2000);
        return false;
    }

    // Check if there's a new version
    if( !OTA_isNewVersion() ) {
        Message(ICON_INFORMATION,  const_cast<char*>("CoolReader"),
            _("You have the latest version."), 2000);
        return false;
    }

    // Get device model number
    const lString16 deviceModel = getPbModelNumber();

    // If download exists
    if( OTA_downloadExists( OTA_genUrl(PB_OTA_URL_MASK_TEST, deviceModel) ) ) {

        // Update
        return OTA_updateFrom( OTA_genUrl(PB_OTA_URL_MASK, deviceModel) );
    }

    // Check if the device is linked to another one
    const lString16 linkedDevice = OTA_getLink(deviceModel);
    if( linkedDevice.empty() ) {
        Message(ICON_WARNING,  const_cast<char*>("CoolReader"),
            (
            lString8(_("Update is not available for your device!\nDevice model: ")) +
            UnicodeToUtf8(deviceModel)
            ).c_str()
            , 5000);
        return false;
    }

    // If the device is linked and the download exists
    if( OTA_downloadExists( OTA_genUrl(PB_OTA_URL_MASK_TEST, linkedDevice) ) ) {

        // Update
        return OTA_updateFrom( OTA_genUrl(PB_OTA_URL_MASK, linkedDevice) );
    }

    // Shouldn't reach this part
    Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
        _("Failed updating!"), 2000);
    return false;
}
