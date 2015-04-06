/**
 * OTA Update implementation
 */

#include <inkview.h>
#include <inkplatform.h>
#include <crengine.h>
#include <crgui.h>
#include <cri18n.h>
#include "web.h"
#include "cr3pocketbook.h"
#include "ota_update.h"

/**
 * The downloading session id
 */
int OTA_sessionId;

/**
 * The OTA branch to use (stable/dev)
 */
lString16 OTA_branch;

/**
 * Replace [BRANCH] in a given URL with the OTA_branch variable
 *
 * @param  url  URL mask
 *
 * @return  final URL
 */
const char * OTA_replaceBranch(lString16 url) {
    url.replace(lString16("[BRANCH]"), OTA_branch);
    return UnicodeToUtf8(url).c_str();
}
const char * OTA_replaceBranch(const char * url) {
    OTA_replaceBranch( lString16(url) );
}

/**
 * Check if there is a new version
 *  - If using dev branch this always returns true
 *
 * @return  true if new version, false otherwise
 */
bool OTA_isNewVersion() {
    if( OTA_branch == lString16(OTA_BRANCH_DEV) )
        return true;

    const char * response = web::get(OTA_replaceBranch(OTA_VERSION)).c_str();
    return
        strlen(response) > 5 &&
        strlen(response) <= OTA_VERSION_MAX_LENGTH &&
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
    const char * response = web::get(OTA_replaceBranch(url)).c_str();
    return
        strlen(response) == strlen(OTA_EXISTS_STR) &&
        strcmp(OTA_EXISTS_STR, response) == 0;
}

/**
 * Gets a device model if the current device is linked
 *
 * @param  deviceModel  current device model number
 *
 * @return  linked device model or empty if not linked
 */
lString16 OTA_getLinkedDevice(const lString16 deviceModel) {
    lString16 url = lString16(OTA_LINK_MASK);
    url.replace(lString16("[DEVICE]"), deviceModel);
    const char * response = web::get(OTA_replaceBranch(url)).c_str();
    if( strlen(response) > 0 && strlen(response) <= OTA_VERSION_MAX_LENGTH )
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
    lString16 url = lString16(OTA_replaceBranch(mask));
    url.replace(lString16("[DEVICE]"), deviceModel);
    return url;
}

void OTA_DL_dialog_handler(int button) {
    return;
}

void OTA_DL_update_progress(void) {
    iv_sessioninfo *si;

    si = GetSessionInfo(OTA_sessionId);

    if (si->length <= 0) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to server!"), 3000);
    } else {
        UpdateProgressbar(_("Downloading..."), 100*si->progress/si->length);
        if (si->progress >= si->length) {
            CloseSession(OTA_sessionId);
            if (si->response >= 400) {
                CloseProgressbar();
                Dialog(ICON_ERROR, const_cast<char*>("CoolReader"), 
                    (
                    lString8(_("Server returned error response: ")) +
                    lString8::itoa((int)si->response) +
                    lString8("\nurl: ") + lString8(si->url) +
                    lString8("\nctype: ") + lString8(si->ctype) +
                    lString8("\nlength: ") + lString8::itoa(si->length) +
                    lString8("\nprogress: ") + lString8::itoa(si->progress)
                    ).c_str(),
                    GetLangText("@Close"), NULL, OTA_DL_dialog_handler);
            } else {
                CloseProgressbar();
                Dialog(ICON_INFORMATION, const_cast<char*>("CoolReader"), 
                    _("Download successfull!\nPlease restart CoolReader."),
                    _("OK"), NULL, OTA_DL_dialog_handler);
            }
        } else {
            SetHardTimer("OTA_DL_update_progress", OTA_DL_update_progress, 1000);
        }
    }
}

/**
 * This does the actual updating of the app
 *
 * @param  url  url
 *
 * @return  true if updated, false if error
 */
bool OTA_updateFrom(const lString16 url) {

    // Remove old package
    iv_unlink(OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME);
    if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) != -1 ) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't remove existing package!"), 2000);
        return false;
    }

    // Download
    OTA_sessionId = NewSession();
    DownloadTo(OTA_sessionId, UnicodeToUtf8(url).c_str(), "",
        OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, 10000);
    SetHardTimer("OTA_DL_update_progress", OTA_DL_update_progress, 1000);

    return true;
}

/**
 * Main func to be called for updating
 *
 * @return  true if updated, false if not
 */
bool OTA_update(const char *branch) {

    // Set used branch
    OTA_branch = lString16(branch);

    iv_dialoghandler progressbar = NULL;

    // Network Connect
    if( !pbNetwork("connect") ) {
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to the network!"), 2000);
        return false;
    }

    OpenProgressbar(ICON_INFORMATION, _("OTA Update"),
        _("Searching..."), 0, progressbar);

    if( !OTA_isNewVersion() ) {
        CloseProgressbar();
        Message(ICON_INFORMATION,  const_cast<char*>("CoolReader"),
            _("You have the latest version."), 2000);
        return false;
    }

    // Get device model number
    const lString16 deviceModel = getPbModelNumber();

    UpdateProgressbar(_("Searching..."), 0);

    // If download exists
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, deviceModel) ) ) {

        UpdateProgressbar(_("Downloading..."), 0);

        // Update
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, deviceModel) );
    }

    UpdateProgressbar(_("Searching..."), 0);

    // Check if the device is linked to another one
    const lString16 linkedDevice = OTA_getLinkedDevice(deviceModel);
    if( linkedDevice.empty() ) {
        CloseProgressbar();
        Message(ICON_WARNING,  const_cast<char*>("CoolReader"),
            (
            lString8(_("Update is not available for your device!\nDevice model: ")) +
            UnicodeToUtf8(deviceModel)
            ).c_str()
            , 5000);
        return false;
    }

    // If the device is linked and the download exists
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, linkedDevice) ) ) {

        UpdateProgressbar(_("Downloading..."), 0);

        // Update
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, linkedDevice) );
    }

    // Shouldn't reach this part
    CloseProgressbar();
    Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
        _("Failed updating!"), 2000);
    return false;
}
