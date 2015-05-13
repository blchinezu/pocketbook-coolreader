#ifndef CR3_POCKETBOOK_H
#define CR3_POCKETBOOK_H

#ifdef __WINE__
#undef LoadBitmap
#define LoadBitmap PB_LoadBitmap

#undef EnumFonts
#define EnumFonts PB_EnumFonts
#endif

#define PB_COMMANDS_START 6200

enum CRPbCommands {
	PB_CMD_BEGIN = PB_COMMANDS_START,
	PB_QUICK_MENU,
	PB_QUICK_MENU_SELECT,
	PB_CMD_ROTATE,
	PB_CMD_ROTATE_ANGLE_SET,
    PB_CMD_CONTENTS,
    PB_CMD_FRONT_LIGHT,
    PB_CMD_FULL_UPDATE,
    PB_CMD_INVERT_DISPLAY,
    PB_CMD_STATUS_LINE,
	PB_CMD_LEFT,
	PB_CMD_RIGHT,
	PB_CMD_UP,
	PB_CMD_DOWN,
	PB_CMD_SELECT_DICT,
	PB_CMD_TRANSLATE,
	PB_CMD_MP3,
	PB_CMD_VOLUME,
	PB_CMD_BOOKMARK_REMOVE,
	PB_CMD_MAIN_MENU,

    PB_CMD_UPDATE_WINDOW,
    PB_CMD_PAGEUP_REPEAT,
    PB_CMD_PAGEDOWN_REPEAT,
    PB_CMD_REPEAT_FINISH,
    PB_CMD_CLEAR_CACHE,

    #ifdef POCKETBOOK_PRO
        PB_CMD_TASK_MANAGER,
        #ifndef POCKETBOOK_PRO_PRO2
            PB_CMD_SYSTEM_PANEL,
        #endif
        PB_CMD_LOCK_DEVICE,
        PB_CMD_OTA_UPDATE,
        PB_CMD_OTA_UPDATE_DEV,
        #ifdef POCKETBOOK_PRO_FW5
            PB_CMD_OPEN_SYSTEM_PANEL,
            PB_CMD_FRONT_LIGHT_TOGGLE,
        #endif
    #endif

    PB_CMD_NONE
};

#define PB_QUICK_MENU_BMP_ID "fbreader_menu"
#define PB_QUICK_MENU_TEXT_ID "qmenu.fbreader.0.text"
#define PB_QUICK_MENU_TEXT_ID_IDX 15

#define PB_QUICK_MENU_ACTION_ID "qmenu.fbreader.0.action"
#define PB_QUICK_MENU_ACTION_ID_IDX 15

#define KEY_BUFFER_LEN 256

#define PROP_POCKETBOOK_ORIENTATION "cr3.pocketbook.orientation"
#define PROP_POCKETBOOK_DICT "cr3.pocketbook.dictionary"
#define PROP_POCKETBOOK_DICT_PAGES "cr3.pocketbook.dict.pages"
#define PROP_POCKETBOOK_DICT_AUTO_TRANSLATE "cr3.pocketbook.dict.auto"
#define PROP_POCKETBOOK_ROTATE_MODE "cr3.pocketbook.rotate_mode"
#define PROP_POCKETBOOK_ROTATE_ANGLE "cr3.pocketbook.rotate_angle"
#define PROP_POCKETBOOK_GRAYBUFFER_BPP "cr3.pocketbook.buffer.bpp"

#define PB_CR3_CACHE_SIZE (0x100000 * 64)

#define CR_PB_VERSION "0.0.6-17-22"
#define CR_PB_BUILD_DATE "2015-05-14"

#define PB_ROTATE_MODE_360 0
#define PB_ROTATE_MODE_180 1
#define PB_ROTATE_MODE_180_SLOW_NEXT 2
#define PB_ROTATE_MODE_180_SLOW_PREV_NEXT 3
#define PB_ROTATE_MODE_180_FAST_NEXT 4
#define PB_ROTATE_MODE_180_FAST_PREV_NEXT 5
#define PB_ROTATE_MODE_180_FAST_NEXT_PREV 6

#define PB_FRESH_UPDATE_MARKER USERDATA"/.cr3updated"
#define PB_DEV_MARKER          USERDATA"/.cr3-use-devel-branch"

#if defined(POCKETBOOK_PRO_FW5)
    #define CR_PB_SDK "pro5"
#elif defined(POCKETBOOK_PRO)
    #if !defined(POCKETBOOK_PRO_PRO2)
        #define CR_PB_SDK "pro4"
    #else
        #define CR_PB_SDK "pro2"
    #endif
#else
    #define CR_PB_SDK "360"
#endif

#ifdef SYSTEMSFRONTLIGHT
    #define PB_FRONT_LIGHT_BIN SYSTEMSFRONTLIGHT
#else
    #define PB_FRONT_LIGHT_BIN SYSTEMDATA"/bin/front-light.app"
#endif

#ifdef NETAGENT
    #define PB_NETWORK_BIN NETAGENT
#else
    #define PB_NETWORK_BIN SYSTEMDATA"/bin/netagent"
#endif

#ifdef AUTO_CONNECT_APP
    #define PB_AUTO_CONNECT_BIN AUTO_CONNECT_APP
#else
    #define PB_AUTO_CONNECT_BIN SYSTEMDATA"/bin/auto_connect.app"
#endif

#define PB_BROWSER_BINARY SYSTEMDATA"/bin/browser.app"
#define PB_BROWSER_EXEC SYSTEMDATA"/bin/openbook"
#define PB_BROWSER_QUERY_GOOGLE "https://www.google.com/search?q="
#define PB_BROWSER_QUERY_WIKIPEDIA "https://en.wikipedia.org/?search="

#define PB_NETWORK_TEST_URL "https://www.google.com"

const char* TR(const char *label);

int getPB_keyboardType();
int getPB_screenType();

bool isGSensorSupported();
bool isFrontLightSupported();
bool isBrowserSupported();

#ifdef POCKETBOOK_PRO
bool isNetworkSupported();
bool pbNetworkConnected();
bool pbNetwork(const char *action);
bool isAutoConnectSupported();
bool isTaskManagerSupported();
#endif

void executeCommand(int commandId, int commandParam);

void pbLaunchWaitBinary(const char *binary, const char *param1, const char *param2, const char *param3);
void pbLaunchWaitBinary(const char *binary, const char *param1, const char *param2);
void pbLaunchWaitBinary(const char *binary, const char *param);
void pbLaunchWaitBinary(const char *binary);

void toggleInvertDisplay();
void toggleStatusLine();
void launchBrowser(lString16 url);
void exitApp();

lString16 getPbModelNumber();

#ifdef POCKETBOOK_PRO
void toggleSystemPanel();
bool systemPanelShown();
#ifdef POCKETBOOK_PRO_FW5
void toggleFrontLight();
#endif
#endif

//#ifndef BACKGROUND_CACHE_FILE_CREATION
//#define BACKGROUND_CACHE_FILE_CREATION
//#endif
#endif //CR3_POCKETBOOK_H

