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
 * Check if there is a new version
 *  - If using dev branch this always returns true
 *
 * @return  true if new version, false otherwise
 */
bool OTA_isNewVersion() {
    CRLog::trace("OTA_isNewVersion()");
    if( OTA_branch == lString16(OTA_BRANCH_DEV) ) {
        CRLog::trace("OTA_isNewVersion(): Dev branch so return true regardless");
        return true;
    }
    lString16 url = lString16(OTA_VERSION);
    url.replace(lString16("[BRANCH]"), OTA_branch);
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    if( strlen(response) > 5 &&
        strlen(response) <= OTA_VERSION_MAX_LENGTH &&
        strcmp(CR_PB_VERSION, response) != 0
        ) {
        CRLog::trace("OTA_isNewVersion(): Found %s", response);
        return true;
    }

    CRLog::trace("OTA_isNewVersion(): Got latest version");
    return false;
}

/**
 * Checks if the test link is fine
 *
 * @param  url  url
 *
 * @return  true if ok, false if not
 */
bool OTA_downloadExists(const lString16 url) {
    CRLog::trace("OTA_downloadExists(%s)", UnicodeToUtf8(url).c_str());
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    if( strlen(response) == strlen(OTA_EXISTS_STR) &&
        strcmp(OTA_EXISTS_STR, response) == 0
        ) {
        CRLog::trace("OTA_downloadExists(): yes");
        return true;
    }
    CRLog::trace("OTA_downloadExists(): no");
    return false;
}

/**
 * Gets a device model if the current device is linked
 *
 * @param  deviceModel  current device model number
 *
 * @return  linked device model or empty if not linked
 */
lString16 OTA_getLinkedDevice(const lString16 deviceModel) {
    CRLog::trace("OTA_getLinkedDevice(%d)", UnicodeToUtf8(deviceModel).c_str());
    lString16 url = lString16(OTA_LINK_MASK);
    url.replace(lString16("[DEVICE]"), deviceModel);
    url.replace(lString16("[BRANCH]"), OTA_branch);
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
    if( strlen(response) > 0 && strlen(response) <= OTA_VERSION_MAX_LENGTH ) {
        CRLog::trace("OTA_getLinkedDevice(): Got device: %s", response);
        return lString16(response);
    }
    CRLog::trace("OTA_getLinkedDevice(): Got no device");
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
    CRLog::trace("OTA_genUrl(%s, %s)", mask, UnicodeToUtf8(deviceModel).c_str());
    lString16 url = lString16(mask);
    url.replace(lString16("[DEVICE]"), deviceModel);
    url.replace(lString16("[BRANCH]"), OTA_branch);
    CRLog::trace("OTA_genUrl(): %s", UnicodeToUtf8(url).c_str());
    return url;
}

void OTA_DL_dialog_handler(int button) {

    // Log
    CRLog::trace("OTA_DL_dialog_handler(%d)", button);
    
    // Mark the required restart
    FILE *marker;
    char buffer[2] = "x";
    marker = fopen(OTA_RESTART_MARK, "wb");
    fwrite(buffer, 1, 1, marker);
    fclose(marker);

    // Exit
    exitApp();
}

void OTA_DL_update_progress(void) {
    CRLog::trace("OTA_DL_update_progress()");
    iv_sessioninfo *si;

    si = GetSessionInfo(OTA_sessionId);

    if (si->length <= 0) {
        CloseProgressbar();
        CRLog::trace("OTA_DL_update_progress(): Couldn't connect to server!");
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to server!"), 3000);
    }
    else {
        int progress = 100*si->progress/si->length;
        CRLog::trace("OTA_DL_update_progress(): Update progress bar @ %d", progress);
        UpdateProgressbar(_("Downloading..."), progress);

        if (si->progress >= si->length) {
            CloseSession(OTA_sessionId);
            CloseProgressbar();
            
            if (si->response >= 400) {
                CRLog::trace("OTA_DL_update_progress(): ERR: url = %s", si->url);
                CRLog::trace("OTA_DL_update_progress(): ERR: ctype = %s", si->ctype);
                CRLog::trace("OTA_DL_update_progress(): ERR: length = %s", si->length);
                CRLog::trace("OTA_DL_update_progress(): ERR: progress = %s", si->progress);

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
            }
            else {
                CRLog::trace("OTA_DL_update_progress(): Done");

                Dialog(ICON_INFORMATION, const_cast<char*>("CoolReader"), 
                    _("Download successful!\nCoolReader will restart itself."),
                    _("OK"), NULL, OTA_DL_dialog_handler);
            }
        }
        else {
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
    CRLog::trace("OTA_updateFrom(%s)", UnicodeToUtf8(url).c_str());

    // Remove old package
    CRLog::trace("OTA_updateFrom(): Remove old package from "OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME);
    iv_unlink(OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME);
    if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) != -1 ) {
        CRLog::trace("OTA_updateFrom(): Couldn't remove existing package!");
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't remove existing package!"), 2000);
        return false;
    }

    // Download
    CRLog::trace("OTA_updateFrom(): Initialize session");
    OTA_sessionId = NewSession();
    CRLog::trace("OTA_updateFrom(): OTA_sessionId = %d", OTA_sessionId);

    CRLog::trace("OTA_updateFrom(): Start download");
    DownloadTo(OTA_sessionId, UnicodeToUtf8(url).c_str(), "",
        OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, 10000);

    CRLog::trace("OTA_updateFrom(): SetHardTimer(OTA_DL_update_progress, 1000)");
    SetHardTimer("OTA_DL_update_progress", OTA_DL_update_progress, 1000);

    return true;
}

/**
 * Main func to be called for updating
 *
 * @return  true launched download
 */
bool OTA_update(const char *branch) {

    CRLog::trace("OTA_update(%s)", branch);

    // Set used branch
    OTA_branch = lString16(branch);

    iv_dialoghandler progressbar = NULL;

    // Network Connect
    if( !pbNetwork("connect") ) {
        CRLog::trace("OTA_update(): Couldn't connect to network");
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to the network!"), 2000);
        return false;
    }

    CRLog::trace("OTA_update(): Open progress bar");
    OpenProgressbar(ICON_INFORMATION, _("OTA Update"),
        _("Searching..."), 0, progressbar);

    CRLog::trace("OTA_update(): Check if there's a new version");
    if( !OTA_isNewVersion() ) {
        CRLog::trace("OTA_update(): No new version");
        CloseProgressbar();
        Message(ICON_INFORMATION,  const_cast<char*>("CoolReader"),
            _("You have the latest version."), 2000);
        return false;
    }

    // Get device model number
    CRLog::trace("OTA_update(): Get device model number");
    const lString16 deviceModel = getPbModelNumber();
    CRLog::trace("OTA_update(): deviceModel = %s", UnicodeToUtf8(deviceModel).c_str());

    UpdateProgressbar(_("Searching..."), 0);

    // If download exists
    CRLog::trace("OTA_update(): Check if download exists");
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, deviceModel) ) ) {

        UpdateProgressbar(_("Downloading..."), 0);

        // Update
        CRLog::trace("OTA_update(): OK, launch OTA_updateFrom");
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, deviceModel) );
    }

    UpdateProgressbar(_("Searching..."), 0);

    // Check if the device is linked to another one
    CRLog::trace("OTA_update(): Check if the device is linked to another one");
    const lString16 linkedDevice = OTA_getLinkedDevice(deviceModel);
    if( linkedDevice.empty() ) {
        CRLog::trace("OTA_update(): Didn't find linked device");
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
    CRLog::trace("OTA_update(): Check if download exists");
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, linkedDevice) ) ) {

        UpdateProgressbar(_("Downloading..."), 0);

        // Update
        CRLog::trace("OTA_update(): OK, launch OTA_updateFrom");
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, linkedDevice) );
    }

    // Shouldn't reach this part
    CRLog::trace("OTA_update(): Download doesn't exist");
    CloseProgressbar();
    Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
        _("Failed updating!"), 2000);
    return false;
}
