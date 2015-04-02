
#include <inkview.h>
#include <inkplatform.h>
#include <crengine.h>
#include <crgui.h>
#include <cri18n.h>
#include "web.h"
#include "cr3pocketbook.h"
#include "ota_update.h"

// #include <unistd.h>
// #include <sys/types.h>
// #include <dirent.h>
// #include <stdio.h>

/*const char * OTA_concat(const char *p1, const char *p2) {
    return (lString8(p1)+lString8(p2)).c_str();
}
const char * OTA_concat(const char *p1, const char *p2, const char *p3) {
    return (lString8(p1)+lString8(p2)+lString8(p3)).c_str();
}*/

void OTA_exec(const char *binary, const char *param1, const char *param2/*, const char *param3*/) {
    pid_t cpid;
    pid_t child_pid;
    cpid = fork();

    switch (cpid) {
        case -1:
            CRLog::error("OTA_exec(): Fork failed!");
            break;

        case 0:
            child_pid = getpid();
            CRLog::trace("OTA_exec(): Child: PID %d", child_pid);
            CRLog::trace("OTA_exec(): Child: Launch %s", binary);
            execl(
                binary,
                binary,
                param1,
                param2,
                /*param3,*/
                NULL
                );
            exit(0);

        default:
            CRLog::trace("OTA_exec(): Parent: Waiting for %d to finish", cpid);
            waitpid(cpid, NULL, 0);
            CRLog::trace("OTA_exec(): Parent: Returned from %s", binary);
    }
}
/*void OTA_exec(const char *binary, const char *param1, const char *param2) {
    OTA_exec(binary, param1, param2, "");
}*/
/*void OTA_exec(const char *binary, const char *param1) {
    OTA_exec(binary, param1, "");
}*/

/*bool OTA_mkdir_recursive(const char *path) {

    if( access( path, F_OK ) != -1 )
        return true;

    OTA_exec(MKDIR_BINARY, "-p", path);
    // mkdir(path, 0777);

    return access( path, F_OK ) != -1;
}*/

bool OTA_rm_recursive(const char *path) {

    if( access( path, F_OK ) == -1 )
        return true;

    OTA_exec(RM_BINARY, "-r", path);

    return access( path, F_OK ) == -1;
}

/*bool OTA_cp_recursive(const char *from, const char *to) {

    if( access( from, F_OK ) == -1 )
        return false;

    if( !OTA_rm_recursive(to) )
        return false;

    OTA_exec(CP_BINARY, "-r", from, to);

    return access( to, F_OK ) != -1;
}*/

/*bool OTA_mv_recursive(const char *from, const char *to) {

    if( access( from, F_OK ) == -1 )
        return false;

    if( !OTA_cp_recursive(from, to) )
        return false;
    
    if( !OTA_rm_recursive(from) )
        return false;

    return access( from, F_OK ) == -1 && access( to, F_OK ) != -1;
}*/

int OTA_sessionId;

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
/*const char * OTA_installPackagePath(const char* path) {

    const char * pathFrom  = (lString8(OTA_TEMP_DIR"/")+lString8(path)).c_str();
    const char * pathTo    = (lString8(FLASHDIR"/")+lString8(path)).c_str();
    const char * pathToBak = (lString8(pathTo)+lString8(".bak")).c_str();

    // If it doesn't exist in the update package
    if( access( pathFrom, F_OK ) == -1 )
        return (
            lString8( "Path not found: " ) +
            lString8(pathFrom)
            ).c_str();

    // Backup old path if exists
    if( access( pathTo, F_OK ) != -1 ) {

        // If backup already exists
        if( access( pathToBak, F_OK ) != -1 && !OTA_rm_recursive(pathToBak) )
            return _("Failed removing old backup!");

        // Move
        if( !OTA_mv_recursive(pathTo, pathToBak) )
            return _("Failed creating backup!");
    }

    // Move the thing, AKA "Install"
    if( !OTA_mv_recursive(pathFrom, pathTo) )
        return _("Failed installing update!");

    // All fine
    return "";
}*/

/**
 * Install the update zip file
 *
 * @param  packagePath  path to the zip file
 *
 * @return  error message or blank
 */
/*const char * OTA_installPackage(const char* packagePath) {

    // Remove old package
    if( access( OTA_TEMP_DIR, F_OK ) != -1 && !OTA_rm_recursive(OTA_TEMP_DIR) )
        return _("Couldn't remove old temporary data!");

    // Extract
    UpdateProgressbar(_("Installing..."), 0);
    if( !OTA_mkdir_recursive(OTA_TEMP_DIR) )
        return _("Failed creating temporary dir!");
    OTA_exec(UNZIP_BINARY, packagePath, "-d", OTA_TEMP_DIR);
    if( access( OTA_INSTALL_SCRIPT, F_OK ) == -1 )
        return _("Extracted package is not valid!");

    // Run installation script
    OTA_exec(SH_BINARY, OTA_INSTALL_SCRIPT);


    // Install binary
    UpdateProgressbar(_("Installing..."), 0);
    const char* msg;
    msg = OTA_installPackagePath("system/bin/cr3-pb.app");
    if( strcmp("", msg) != 0 )
        return msg;
    msg = OTA_installPackagePath("system/share/cr3");
    if( strcmp("", msg) != 0 )
        return msg;

    // Remove temp data
    UpdateProgressbar(_("Installing..."), 0);
    if( access( OTA_TEMP_DIR, F_OK ) != -1 && !OTA_rm_recursive(OTA_TEMP_DIR) )
        return _("Couldn't remove temporary data!");

    // All fine
    return "";
}*/

/*bool OTA_updateFrom_continue() {

    // Failed download
    if( access( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME, F_OK ) == -1 ) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't download the update package!"), 3000);
        return false;
    }

    // Install
    UpdateProgressbar(_("Installing..."), 0);
    const char* msg = OTA_installPackage( OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME );
    if( strcmp("", msg) != 0 ) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"), msg, 2000);
        return false;
    }

    // Remove package
    if( !OTA_rm_recursive(OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME) ) {
        CloseProgressbar();
        Message(ICON_WARNING,  const_cast<char*>("CoolReader"),
            _("Couldn't remove downloaded package!"), 2000);
        return true;
    }

    // Success
    UpdateProgressbar(_("Update successfull!"), 0);
    sleep(3);
    UpdateProgressbar(_("CoolReader will reload itself..."), 0);
    sleep(3);
    CloseProgressbar();

    // Reload app


    return true;
}*/

void OTA_DL_dialog_handler(int button) {
    return;
    // OTA_updateFrom_continue();
}

void OTA_DL_update_progress(void) {
    iv_sessioninfo *si;

    si = GetSessionInfo(OTA_sessionId);

    if (si->length <= 0) {
        CloseProgressbar();
        Message(ICON_ERROR,  const_cast<char*>("CoolReader"),
            _("Couldn't connect to server!"), 3000);
        // OTA_updateFrom_continue();
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
                // OTA_updateFrom_continue();
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
    if( !OTA_rm_recursive(OTA_DOWNLOAD_DIR"/"OTA_PACKAGE_NAME) ) {
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
bool OTA_update() {

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
