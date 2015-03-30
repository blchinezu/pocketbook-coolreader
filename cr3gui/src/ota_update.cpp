
#include <inkview.h>
#include <inkplatform.h>
#include <crengine.h>
#include <crgui.h>
#include <cri18n.h>
#include "web.h"
#include "cr3pocketbook.h"
#include "ota_update.h"


int OTA_sessionId;


/**
 * Update progress bar
 *
 * @param  text         text shown
 * @param  progress     0..100
 * @param  deviceModel  device model
 */
void OTA_progress(const char *text, int progress, lString16 deviceModel) {
    lString16 finalText = lString16(text);
    finalText.replace(lString16("[DEVICE]"), deviceModel);
    UpdateProgressbar(UnicodeToUtf8(finalText).c_str(), progress);
}
void OTA_progress(const char *text, int progress) {
    OTA_progress(text, progress, lString16(""));
}

/**
 * Check if there is a new version
 *
 * @return  true if new version, false otherwise
 */
bool OTA_isNewVersion() {
    const char * response = web::get(OTA_VERSION).c_str();
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
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
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
    const char * response = web::get(UnicodeToUtf8(url).c_str()).c_str();
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
    lString16 url = lString16(mask);
    url.replace(lString16("[DEVICE]"), deviceModel);
    return url;
}

/**
 * Install a certain path from the update zip package
 *
 * @param  path  path to "install"
 *
 * @return  error message or blank
 */
const char * OTA_installPackagePath(const char* path) {

    const char * pathFrom  = (lString8(OTA_TEMP_DIR"/")+lString8(path)).c_str();
    const char * pathTo    = (lString8(FLASHDIR"/")+lString8(path)).c_str();
    const char * pathToBak = (lString8(pathTo)+lString8(".bak")).c_str();

    // If it doesn't exist in the update package
    if( access( pathFrom, F_OK ) == -1 )
        return _("Invalid update package!");

    // Backup old path if exists
    if( access( pathTo, F_OK ) != -1 ) {

        // If backup already exists
        if( access( pathToBak, F_OK ) != -1 ) {
            pbLaunchWaitBinary(RM_BINARY, "-rf", pathToBak);
            if( access( pathToBak, F_OK ) != -1 )
                return _("Failed removing old backup!");
        }

        // Move
        pbLaunchWaitBinary(MV_BINARY, pathTo, pathToBak);
        if( access( pathTo, F_OK ) != -1 )
            return _("Failed creating backup!");
    }

    // Move the thing, AKA "Install"
    pbLaunchWaitBinary(MV_BINARY, pathFrom, pathTo);
    if( access( pathTo, F_OK ) != -1 )
        return _("Failed installing update!");

    // All fine
    return "";
}

/**
 * Install the update zip file
 *
 * @param  packagePath  path to the zip file
 *
 * @return  error message or blank
 */
const char * OTA_installPackage(const char* packagePath) {

    // Remove old package
    if( access( OTA_TEMP_DIR, F_OK ) != -1 ) {
        pbLaunchWaitBinary(RM_BINARY, "-rf", OTA_TEMP_DIR);
        if( access( OTA_TEMP_DIR, F_OK ) != -1 ) {
            return _("Couldn't remove old temporary data!");
        }
    }

    // Extract
    OTA_progress(_("Installing..."), 70);
    pbLaunchWaitBinary(MKDIR_BINARY, "-p", OTA_TEMP_DIR);
    if( access( OTA_TEMP_DIR, F_OK ) == -1 )
        return _("Failed creating temporary dir!");
    pbLaunchWaitBinary(UNZIP_BINARY, "-d", OTA_TEMP_DIR, packagePath);
    if( access( OTA_TEMP_DIR"/system/bin", F_OK ) == -1 )
        return _("Invalid update package!");

    // Install binary
    OTA_progress(_("Installing..."), 80);
    const char* msg;
    msg = OTA_installPackagePath("system/bin/cr3-pb.app");
    if( strcmp("", msg) != 0 )
        return msg;
    msg = OTA_installPackagePath("system/share/cr3");
    if( strcmp("", msg) != 0 )
        return msg;

    // Remove temp data
    OTA_progress(_("Installing..."), 90);
    if( access( OTA_TEMP_DIR, F_OK ) != -1 ) {
        pbLaunchWaitBinary(RM_BINARY, "-rf", OTA_TEMP_DIR);
        if( access( OTA_TEMP_DIR, F_OK ) != -1 ) {
            return _("Couldn't remove temporary data!");
        }
    }

    // All fine
    return "";
}

bool OTA_updateFrom_continue();
void OTA_DL_dialog_handler(int button) {
    OTA_updateFrom_continue();
}

void OTA_DL_update_progress(void) {
    iv_sessioninfo *si;

    si = GetSessionInfo(OTA_sessionId);

    if (si->length <= 0) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to server!"), 3000);
        OTA_updateFrom_continue();
    } else {
        if (si->progress >= si->length) {
            CloseSession(OTA_sessionId);
            CloseProgressbar();
            if (si->response >= 400) {
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
                OTA_updateFrom_continue();
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
    if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) != -1 ) {
        iv_unlink(OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME);
        if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) != -1 ) {
            CloseProgressbar();
            Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
                _("Couldn't remove existing package!"), 2000);
            return false;
        }
    }

    // Download
    OTA_sessionId = NewSession();
    DownloadTo(OTA_sessionId, UnicodeToUtf8(url).c_str(), "",
        OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, 10000);
    SetHardTimer("OTA_DL_update_progress", OTA_DL_update_progress, 1000);

    return true;
}
bool OTA_updateFrom_continue() {

    // Failed download
    if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) == -1 ) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't download the update package!"), 3000);
        return false;
    }

    // Install
    OTA_progress(_("Installing..."), 60);
    const char* msg = OTA_installPackage( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME );
    if( strcmp("", msg) != 0 ) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"), msg, 2000);
        return false;
    }

    // Remove package
    if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) != -1 ) {
        iv_unlink(OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME);
        if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) != -1 ) {
            CloseProgressbar();
            Message(ICON_WARNING,  const_cast<char*>("CoolReader"),
                _("Couldn't remove downloaded package!"), 2000);
            return true;
        }
    }

    return true;
}

/**
 * Main func to be called for updating
 *
 * @return  true if updated, false if not
 */
bool OTA_update() {

    iv_dialoghandler progressbar;

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

    OTA_progress(_("Searching..."), 20);

    // If download exists
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, deviceModel) ) ) {

        OTA_progress(_("Downloading package for [DEVICE]..."), 50, deviceModel);

        // Update
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, deviceModel) );
    }

    OTA_progress(_("Searching..."), 30);

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

    OTA_progress(_("Searching..."), 40, linkedDevice);

    // If the device is linked and the download exists
    if( OTA_downloadExists( OTA_genUrl(OTA_URL_MASK_TEST, linkedDevice) ) ) {

        OTA_progress(_("Downloading package for [DEVICE]..."), 50, linkedDevice);

        // Update
        return OTA_updateFrom( OTA_genUrl(OTA_URL_MASK, linkedDevice) );
    }

    // Shouldn't reach this part
    CloseProgressbar();
    Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
        _("Failed updating!"), 2000);
    return false;
}
