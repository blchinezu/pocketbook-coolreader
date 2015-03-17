//
// C++ Implementation: settings
//
// Description:
//
//
// Author: Vadim Lopatin <vadim.lopatin@coolreader.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "settings.h"
#include <crgui.h>
#include "viewdlg.h"
#include "mainwnd.h"
//#include "fsmenu.h"
#ifdef CR_POCKETBOOK
#include "cr3pocketbook.h"
#endif
#include <inkview.h>
#ifdef POCKETBOOK_PRO
#include <inkplatform.h>
#endif

#include <cri18n.h>

#define CR3_ACTION_NONE "NONE"
#define CR3_ACTION_EXIT "EXIT"
#define CR3_ACTION_PAGE_DOWN "PAGE_DOWN"
#define CR3_ACTION_PAGE_DOWN_10 "PAGE_DOWN_10"
#define CR3_ACTION_PAGE_UP "PAGE_UP"
#define CR3_ACTION_PAGE_UP_10 "PAGE_UP_10"
#define CR3_ACTION_ZOOM_IN "ZOOM_IN"
#define CR3_ACTION_ZOOM_OUT "ZOOM_OUT"
#define CR3_ACTION_BOOKMARKS "BOOKMARKS"
#define CR3_ACTION_MAIN_MENU "MAIN_MENU"
#define CR3_ACTION_ABOUT "ABOUT"
#define CR3_ACTION_SEARCH "SEARCH"
#define CR3_ACTION_DICT "DICT"
#define CR3_ACTION_SETTINGS "SETTINGS"
#define CR3_ACTION_GO_PAGE "GO_PAGE"
#define CR3_ACTION_GO_LINK "GO_LINK"
#define CR3_ACTION_GO_FIRST "GO_FIRST"
#define CR3_ACTION_GO_LAST "GO_LAST"
#define CR3_ACTION_CITES "CITES"
#define CR3_ACTION_QUICK_MENU "QUICK_MENU"
#define CR3_ACTION_PB_MAIN_MENU "PB_MAIN_MENU"
#define CR3_ACTION_PB_CONTENTS "PB_CONTENTS"
#define CR3_ACTION_PB_FRONT_LIGHT "PB_FRONT_LIGHT"
#define CR3_ACTION_PB_TASK_MANAGER "PB_TASK_MANAGER"
#define CR3_ACTION_PB_INVERT_DISPLAY "PB_INVERT_DISPLAY"
#define CR3_ACTION_PB_STATUS_LINE "PB_STATUS_LINE"
#ifdef POCKETBOOK_PRO
#define CR3_ACTION_PB_SYSTEM_PANEL "PB_SYSTEM_PANEL"
#define CR3_ACTION_PB_LOCK_DEVICE "PB_LOCK_DEVICE"
#endif

extern lString16 pbSkinFileName;

typedef struct {
    const char * action_id;
    const int commandId;
    const int commandParam;
} action_def_t;

static const action_def_t availableActions[] = {
    { CR3_ACTION_NONE, 0, 0 },
    { CR3_ACTION_EXIT, MCMD_QUIT, 0 },
    { CR3_ACTION_PAGE_DOWN, DCMD_PAGEDOWN, 1 },
    { CR3_ACTION_PAGE_DOWN_10, DCMD_PAGEDOWN, 10 },
    { CR3_ACTION_PAGE_UP, DCMD_PAGEUP, 1 },
    { CR3_ACTION_PAGE_UP_10, DCMD_PAGEUP, 10 },
    { CR3_ACTION_ZOOM_IN, DCMD_ZOOM_IN, 0 },
    { CR3_ACTION_ZOOM_OUT, DCMD_ZOOM_OUT, 0 },
    { CR3_ACTION_BOOKMARKS, MCMD_BOOKMARK_LIST, 0 },
    { CR3_ACTION_MAIN_MENU, MCMD_MAIN_MENU, 0 },
    { CR3_ACTION_ABOUT, MCMD_ABOUT, 0 },
    { CR3_ACTION_SEARCH, MCMD_SEARCH, 0 },
    { CR3_ACTION_DICT, MCMD_DICT, 0 },
    { CR3_ACTION_SETTINGS, MCMD_SETTINGS, 0 },
    { CR3_ACTION_GO_PAGE, MCMD_GO_PAGE, 0 },
    { CR3_ACTION_GO_LINK, MCMD_GO_LINK, 0 },
    { CR3_ACTION_GO_FIRST, DCMD_BEGIN, 0 },
    { CR3_ACTION_GO_LAST, DCMD_END, 0 },
    { CR3_ACTION_CITES, MCMD_CITE, 0 },
#ifdef CR_POCKETBOOK
    { CR3_ACTION_QUICK_MENU, PB_QUICK_MENU, 0 },
    { CR3_ACTION_PB_MAIN_MENU, PB_CMD_MAIN_MENU, 0 },
    { CR3_ACTION_PB_CONTENTS, PB_CMD_CONTENTS, 0 },
    { CR3_ACTION_PB_FRONT_LIGHT, PB_CMD_FRONT_LIGHT, 0 },
    // { CR3_ACTION_PB_TASK_MANAGER, PB_CMD_TASK_MANAGER, 0 },
    { CR3_ACTION_PB_INVERT_DISPLAY, PB_CMD_INVERT_DISPLAY, 0 },
    { CR3_ACTION_PB_STATUS_LINE, PB_CMD_STATUS_LINE, 0 },
    #ifdef POCKETBOOK_PRO
    { CR3_ACTION_PB_SYSTEM_PANEL, PB_CMD_SYSTEM_PANEL, 0 },
    // { CR3_ACTION_PB_LOCK_DEVICE, PB_CMD_LOCK_DEVICE, 0 },
    #endif
#endif
    { NULL, 0, 0 }
};

static const char* default_actions_short[] = {
    CR3_ACTION_GO_LINK, CR3_ACTION_DICT, CR3_ACTION_NONE,
    CR3_ACTION_PAGE_UP, CR3_ACTION_QUICK_MENU, CR3_ACTION_PAGE_DOWN,
    CR3_ACTION_CITES, CR3_ACTION_BOOKMARKS, CR3_ACTION_NONE
};

static const char* default_actions_long[] = {
    CR3_ACTION_NONE, CR3_ACTION_NONE, CR3_ACTION_NONE,
    CR3_ACTION_PAGE_UP_10, CR3_ACTION_MAIN_MENU, CR3_ACTION_PAGE_DOWN_10,
    CR3_ACTION_NONE, CR3_ACTION_NONE, CR3_ACTION_NONE
};

class CRControlsMenu;
class CRControlsMenuItem : public CRMenuItem
{
private:
    int _key;
    int _flags;
    int _command;
    int _params;
    CRControlsMenu * _controlsMenu;
    lString16 _settingKey;
    int _defCommand;
    int _defParams;
public:
    bool getCommand( int & cmd, int & params )
    {
        lString16 v = _menu->getProps()->getStringDef(LCSTR(_settingKey), "");
        cmd = _defCommand;
        params = _defParams;
        return splitIntegerList( v, lString16(","), cmd, params );
   }
    /// submenu for options dialog support
    virtual lString16 getSubmenuValue()
    {
        int cmd;
        int params;
        bool isSet = getCommand( cmd, params );
        if(isSet) {
            lString16 res = Utf8ToUnicode(lString8(getCommandName( cmd, params )));
            // TODO: use default flag
            return res;
        }
        return lString16::empty_str;
    }
    /// called on item selection
    virtual int onSelect();
    CRControlsMenuItem( CRControlsMenu * menu, int id, int key, int flags, const CRGUIAccelerator * defAcc );
    virtual void Draw( LVDrawBuf & buf, lvRect & rc, CRRectSkinRef skin, CRRectSkinRef valueSkin, bool selected );
};

class CRControlsMenu : public CRFullScreenMenu
{
    lString16 _accelTableId;
    CRGUIAcceleratorTableRef _baseAccs;
    CRGUIAcceleratorTableRef _overrideCommands;
    lString16 _settingKey;
public:
    CRPropRef getProps() { return _props; }
    lString16 getSettingKey( int key, int flags )
    {
        lString16 res = _settingKey;
        if ( key!=0 )
            res << "." << fmt::decimal(key) << "." << fmt::decimal(flags);
        return res;
    }
    lString16 getSettingLabel( int key, int flags )
    {
        return lString16(getKeyName(key, flags));
    }
    CRMenu * createCommandsMenu(int key, int flags)
    {
        lString16 label = getSettingLabel( key, flags ) + " - " + lString16(_("choose command"));
        lString16 keyid = getSettingKey( key, flags );
        CRMenu * menu = new CRMenu(_wm, this, _id, label, LVImageSourceRef(), LVFontRef(), LVFontRef(), _props, LCSTR(keyid), 8);
        for ( int i=0; i<_overrideCommands->length(); i++ ) {
            const CRGUIAccelerator * acc = _overrideCommands->get(i);
            lString16 cmdLabel = lString16( getCommandName(acc->commandId, acc->commandParam) );
            lString16 cmdValue = lString16::itoa(acc->commandId) << "," << lString16::itoa(acc->commandParam);
            CRMenuItem * item = new CRMenuItem( menu, i, cmdLabel, LVImageSourceRef(), LVFontRef(), cmdValue.c_str());
            menu->addItem(item);
        }
        menu->setAccelerators( getAccelerators() );
        menu->setSkinName(lString16("#settings"));
        menu->setValueFont(_valueFont);
        menu->setFullscreen(true);
        menu->reconfigure( 0 );
        return menu;
    }
    CRControlsMenu(CRMenu * baseMenu, int id, CRPropRef props, lString16 accelTableId, int numItems, lvRect & rc)
    : CRFullScreenMenu( baseMenu->getWindowManager(), id, lString16(_("Controls")), numItems, rc )
    {
        _menu = baseMenu;
        _props = props;
        _baseAccs = _wm->getAccTables().get( accelTableId );
        if (_baseAccs.isNull()) {
            CRLog::error("CRControlsMenu: No accelerators %s", LCSTR(_accelTableId) );
        }
        _accelTableId = accelTableId;
        CRGUIAcceleratorTableRef _overrideKeys = _wm->getAccTables().get( accelTableId + "-override-keys" );
        if ( _overrideKeys.isNull() ) {
            CRLog::error("CRControlsMenu: No table of allowed keys for override accelerators %s", LCSTR(_accelTableId) );
            return;
        }
        _overrideCommands = _wm->getAccTables().get( accelTableId + "-override-commands" );
        if ( _overrideCommands.isNull() ) {
            CRLog::error("CRControlsMenu: No table of allowed commands to override accelerators %s", LCSTR(_accelTableId) );
            return;
        }
        _settingKey = lString16("keymap.") + _accelTableId;
        for ( int i=0; i<_overrideKeys->length(); i++ ) {
            const CRGUIAccelerator * acc = _overrideKeys->get(i);
            CRControlsMenuItem * item = new CRControlsMenuItem(this, i, acc->keyCode, acc->keyFlags,
                         _baseAccs->findKeyAccelerator( acc->keyCode, acc->keyFlags ) );
            addItem(item);
        }
    }

    virtual bool onCommand( int command, int params )
    {
        switch ( command ) {
        case mm_Controls:
            return true;
        default:
            return CRMenu::onCommand( command, params );
        }
    }
};

/// called on item selection
int CRControlsMenuItem::onSelect()
{
    CRMenu * menu = _controlsMenu->createCommandsMenu(_key, _flags);
    _menu->getWindowManager()->activateWindow(menu);
    return 1;
}

class CRActionsMenu : public CRFullScreenMenu
{
public:
    CRActionsMenu(CRMenu * baseMenu, CRPropRef props, lvRect &rc)
    : CRFullScreenMenu( baseMenu->getWindowManager(), 0, lString16(), 8, rc )
    {
        _menu = baseMenu;
        _props = props;
    }
    void setPropertyName(lString16 propertyName)
    {
        _propName = propertyName;
    }
};

class CRTapZoneSettings : public CRFullScreenMenu
{
private:
    const char *_propertyName;
    int x0,x1,x2,x3;
protected:
    CRPropRef _props;
    CRActionsMenu *_actionsMenu;
    virtual void drawClient();
    const char * getActionName(int index);
    int getActionIndex(const char *actionId);
    lString16 getActionNameForTapZone(int tapZone);
    void drawTapZonesRow(LVDrawBuf & buf, CRRectSkinRef clientSkin, int row,
                         int rowStart, int rowEnd);
public:
    CRTapZoneSettings(CRMenu * parentMenu, int id, const lString16 & label, CRPropRef props,
                      const char *propName, lvRect & rc) :
        CRFullScreenMenu( parentMenu->getWindowManager(), id,  label, 1, rc ),
        _propertyName(propName), _props(props)
    {
        setSkinName(lString16("#tapSettings"));
        setAccelerators(parentMenu->getAccelerators());
        _actionsMenu = new CRActionsMenu(this, _props, rc);
        for ( int i=0; availableActions[i].action_id; i++ ) {

            #ifdef CR_POCKETBOOK
            if( availableActions[i].action_id == CR3_ACTION_PB_FRONT_LIGHT &&
                !isFrontLightSupported() )
                continue;
            if( availableActions[i].action_id == CR3_ACTION_PB_TASK_MANAGER &&
                !isTaskManagerSupported() )
                continue;
            #endif

            lString8 label( getActionName(i) );
            lString8 propertyValue( availableActions[i].action_id);
            _actionsMenu->addItem( new CRMenuItem(_actionsMenu, i,
                                                  label.c_str(),
                                                  LVImageSourceRef(), LVFontRef(),
                                                  Utf8ToUnicode( propertyValue ).c_str()) );
        }
        _actionsMenu->setAccelerators( parentMenu->getAccelerators() );
        _actionsMenu->setSkinName(lString16("#settings"));
        _actionsMenu->reconfigure( 0 );
    }

    virtual bool onCommand( int command, int params)
    {
        switch (command) {
        case MCMD_CANCEL:
            closeMenu( 0 );
            return true;
        case MCMD_PREV_PAGE:
        case MCMD_OK:
            doCloseMenu(getId(), false, params);
            return true;
        case MCMD_NEXT_ITEM:
        case MCMD_PREV_ITEM:
        case MCMD_NEXT_PAGE:
            return true;
        }
        return CRMenu::onCommand( command, params );
    }

    virtual bool onClientTouch(lvPoint &pt, CRGUITouchEventType evType)
    {
        bool ret = false;
        int selectedZone = getTapZone(pt.x, pt.y, _props);
        if (CRTOUCH_UP == evType || CRTOUCH_DOWN_LONG == evType) {
            _actionsMenu->setPropertyName( lString16(_propertyName).appendDecimal(selectedZone));
            _actionsMenu->setLabel( getLabel() );
            _wm->activateWindow(_actionsMenu);
            ret = true;
        }
        return ret;
    }
};

class CRTapZoneSettingsMenuItem : public CRMenu
{
private:
    const char *_propName;
public:
    CRTapZoneSettingsMenuItem(CRMenu * parentMenu, CRPropRef props, const char * label, const char * propName)
        : CRMenu(parentMenu->getWindowManager(), parentMenu, mm_touchScreenZones, label, LVImageSourceRef(), LVFontRef(), LVFontRef()),
          _propName(propName)
    {
        _props = props;
    }
    int onSelect()
    {
        CRTapZoneSettings *menu = new CRTapZoneSettings(_menu, mm_touchScreenZones, getLabel(), _props,
                                                        _propName, _rect );
        _wm->activateWindow(menu);
        return 1;
    }
};

const char * CRTapZoneSettings::getActionName(int index)
{
    const char * name;
    if ( index == 0 )
        name = _("(No action)");
    else
        name = getCommandName( availableActions[index].commandId,
                              availableActions[index].commandParam );
    return name;
}

int CRTapZoneSettings::getActionIndex(const char *actionId)
{
    lString16 id(actionId);
    int ret = 0;
    for ( int i=0; availableActions[i].action_id; i++ ) {
        if ( id == availableActions[i].action_id ) {
            ret = i;
            break;
        }
    }
    return ret;
}

lString16 CRTapZoneSettings::getActionNameForTapZone(int tapZone)
{
    lString16 value = _props->getStringDef(lString8(_propertyName).appendDecimal(tapZone).c_str(),
                                           CR3_ACTION_NONE);
    return lString16(getActionName(getActionIndex(LCSTR(value))));

}

void CRTapZoneSettings::drawTapZonesRow(LVDrawBuf & buf, CRRectSkinRef clientSkin,
                                        int row, int rowStart, int rowEnd)
{
    int startZone = row * 3 + 1;
    if ( x1 > x0 ) {
        lvRect tapRc(x0,rowStart,x1,rowEnd);
        clientSkin->drawText(buf, tapRc,
                             getActionNameForTapZone(startZone));
    }
    if ( x2 > x1 ) {
        lvRect tapRc(x1,rowStart,x2,rowEnd);
        clientSkin->drawText(buf, tapRc,
                             getActionNameForTapZone(startZone + 1));
    }
    if ( x2 < x3 ) {
        lvRect tapRc(x2,rowStart,x3,rowEnd);
        clientSkin->drawText(buf, tapRc,
                             getActionNameForTapZone(startZone + 2));
    }
}

void CRTapZoneSettings::drawClient()
{
    lvRect rc;
    if ( !_pageUpdate || !getClientRect( rc ) )
        return;

    LVDrawBuf & buf = *_wm->getScreen()->getCanvas();
    CRMenuSkinRef skin = getSkin();
    CRRectSkinRef clientSkin = skin->getClientSkin();

    lUInt32 cl = 0x000000;
    if ( !clientSkin.isNull() ) {
        clientSkin->draw( buf, rc );
        cl = clientSkin->getTextColor();
        x0 = rc.left;
        x1 = x0 + rc.width() * _props->getIntDef(PROP_TAP_ZONE_WIDTH_1, 33) / 100;
        x2 = x1 + rc.width() * _props->getIntDef(PROP_TAP_ZONE_WIDTH_2, 33) / 100;
        x3 = rc.width();
        int y = rc.top;
        int y1 = y + rc.height() * _props->getIntDef(PROP_TAP_ZONE_HEIGHT_1, 33) / 100;
        int y2 = y1 + rc.height() * _props->getIntDef(PROP_TAP_ZONE_HEIGHT_2, 33) / 100;
        if ( x1 > x0 && x1 < rc.width() )
            buf.FillRect(x1, rc.top, x1+1, rc.bottom, cl);
        if ( x2 > x1 && x2 < rc.width() )
            buf.FillRect(x2, rc.top, x2+1, rc.bottom, cl);
        if ( y1 > y ) {
            if ( y1 < rc.height() )
                buf.FillRect(rc.left, y1, rc.right, y1+1, cl);
            drawTapZonesRow(buf, clientSkin, 0, y, y1);
        }
        if ( y2 > y1 ) {
            if ( y2 < rc.height())
                buf.FillRect(rc.left, y2, rc.right, y2+1, cl);
            drawTapZonesRow(buf, clientSkin, 1, y1, y2);
        }
        if (y2 < rc.height()) {
            drawTapZonesRow(buf, clientSkin, 2, y2, rc.height());
        }
    } else {
        buf.FillRect(rc, 0xFFFFFF);
    }
    if (!_controlsCreated)
        addControl(new CRClientControl(this, rc));
}

void initTapDefaultActions(CRPropRef props)
{

    for( unsigned i=0; i < sizeof(default_actions_short)/sizeof(default_actions_short[0]); i++ ) {
        lString8 property(PROP_TAP_ZONE_ACTION);
        property.appendDecimal(i + 1);
        props->setStringDef(property.c_str(), default_actions_short[i]);
    }
    for( unsigned i=0; i < sizeof(default_actions_long)/sizeof(default_actions_long[0]); i++ ) {
        lString8 property(PROP_TAP_ZONE_ACTION_LONG);
        property.appendDecimal(i + 1);
        props->setStringDef(property.c_str(), default_actions_long[i]);
    }
}

void getCommandForTapZone(int tapZone, CRPropRef props, bool longTap, int &command, int &param)
{
    lString8 property;
    if ( longTap ) {
        property = PROP_TAP_ZONE_ACTION_LONG;
    } else {
        property = PROP_TAP_ZONE_ACTION;
    }
    property.appendDecimal(tapZone);
    lString16 value = props->getStringDef(property.c_str(), CR3_ACTION_NONE);
    command = param = 0;
    for ( int i=0; availableActions[i].action_id; i++ ) {
        if ( value == availableActions[i].action_id ) {
            command = availableActions[i].commandId;
            param = availableActions[i].commandParam;
            break;
        }
    }
}

CRControlsMenuItem::CRControlsMenuItem( CRControlsMenu * menu, int id, int key, int flags, const CRGUIAccelerator * defAcc )
: CRMenuItem(menu, id, getKeyName(key, flags), LVImageSourceRef(), LVFontRef() ), _key( key ), _flags(flags)
{
    _defCommand = _defParams = 0;
    if ( defAcc ) {
        _defCommand = defAcc->commandId;
        _defParams = defAcc->commandParam;
    }
    _controlsMenu = menu;
    _settingKey = menu->getSettingKey( key, flags );
}

void CRControlsMenuItem::Draw( LVDrawBuf & buf, lvRect & rc, CRRectSkinRef skin, CRRectSkinRef valueSkin, bool selected )
{
    lvRect itemBorders = skin->getBorderWidths();
    skin->draw( buf, rc );
    buf.SetTextColor( skin->getTextColor() );
    buf.SetBackgroundColor( skin->getBackgroundColor() );
    lvRect textRect = rc;
    lvRect borders = skin->getBorderWidths();
    //textRect.shrinkBy(borders);
    skin->drawText(buf, textRect, _label);
    lString16 s = getSubmenuValue();
    if ( s.empty() )
        s = lString16(_("[Command is not assigned]"));
    //_menu->getValueFont()->DrawTextString( &buf, rc.right - w - 8, rc.top + hh/2 - _menu->getValueFont()->getHeight()/2, s.c_str(), s.length(), L'?', NULL, false, 0 );
    valueSkin->drawText(buf, textRect, s);
}

bool CRSettingsMenu::onCommand( int command, int params )
{
    switch ( command ) {
    case mm_Controls:
        {
        }
        return true;
    default:
        return CRMenu::onCommand( command, params );
    }
}

#if CR_INTERNAL_PAGE_ORIENTATION==1 || defined(CR_POCKETBOOK)
CRMenu * CRSettingsMenu::createOrientationMenu( CRMenu * mainMenu, CRPropRef props )
{
	item_def_t page_orientations[] = {
		{_("0` (Portrait)"), "0"},
		{_("90 `"), "1"},
		{_("180 `"), "2"},
		{_("270 `"), "3"},
		{NULL, NULL},
	};

    LVFontRef valueFont( fontMan->GetFont( VALUE_FONT_SIZE, 400, true, css_ff_sans_serif, lString8("Arial")) );
#ifdef CR_POCKETBOOK
	const char * propName = PROP_POCKETBOOK_ORIENTATION;
#else
	const char * propName = PROP_ROTATE_ANGLE;
#endif
    CRMenu * orientationMenu = new CRMenu(_wm, mainMenu, mm_Orientation,
            lString16(_("Page orientation")),
                            LVImageSourceRef(), LVFontRef(), valueFont, props,  propName);
    addMenuItems( orientationMenu, page_orientations );
    orientationMenu->reconfigure( 0 );

    return orientationMenu;
}
#endif

DECL_DEF_CR_FONT_SIZES;
static int cr_status_font_sizes[MAX_STATUS_FONT_SIZE - MIN_STATUS_FONT_SIZE + 1];

class FontSizeMenu : public CRMenu
{
public:
    FontSizeMenu(  CRGUIWindowManager * wm, CRMenu * parentMenu, LVFontRef valueFont, CRPropRef props, const char * propName  )
    : CRMenu( wm, parentMenu, mm_FontSize,
                                _("Default font size"),
                                        LVImageSourceRef(), LVFontRef(), valueFont, props, propName, 10 )
    {
        _fullscreen = true;
    }

    /// submenu for options dialog support
    virtual lString16 getSubmenuValue()
    { 
        return getProps()->getStringDef(
            UnicodeToUtf8(getPropName()).c_str(), "32");
    }
};

class OnDemandFontMenuItem : public CRMenuItem
{
    int _size;
    int _weight;
    bool _italic;
    css_font_family_t _family;
    lString8 _typeface;
    lString8 _deftypeface;
    public:
    OnDemandFontMenuItem( CRMenu * menu, int id, lString16 label, LVImageSourceRef image, const lChar16 * propValue,
                          int size, int weight, bool italic, css_font_family_t family, lString8 typeface, lString8 deftypeface )
    : CRMenuItem(menu, id, LCSTR(label), image, LVFontRef(), propValue)
    , _size(size), _weight(weight), _italic(italic), _family(family), _typeface(typeface), _deftypeface(deftypeface)
    {
    }
    /// item label font
    virtual LVFontRef getFont()
    {
        if ( _defFont.isNull() ) {
            CRLog::trace("Creating font %s[%d] on demand", _typeface.c_str(), _size);
            _defFont = fontMan->GetFont( _size, _weight, _italic, _family, _typeface);
            LVFont::glyph_info_t glyph;
            if ( !_defFont->getGlyphInfo('A', &glyph) ) {
                _defFont = fontMan->GetFont( _size, _weight, _italic, _family, _deftypeface);
            }
        }
        return _defFont;
    }
};

CRMenu * CRSettingsMenu::createFontSizeMenu( CRMenu * mainMenu, int *fontSizes, unsigned sizesCount,
                                            CRPropRef props, const char * propName )
{
    lString16Collection list;
    fontMan->getFaceList( list );
    lString8 fontFace = UnicodeToUtf8(props->getStringDef( PROP_FONT_FACE, UnicodeToUtf8(list[0]).c_str() ));
    CRMenuSkinRef skin = _wm->getSkin()->getMenuSkin(L"#settings");
    LVFontRef valueFont = skin->getValueSkin()->getFont();
    CRMenu * fontSizeMenu;
    fontSizeMenu = new FontSizeMenu(_wm, mainMenu, valueFont, props, propName );
    for ( unsigned i=0; i<sizesCount; i++ ) {
        char defvalue[400];
        sprintf( defvalue, "%d %s", fontSizes[i], _("The quick brown fox jumps over lazy dog") );
        fontSizeMenu->addItem( new OnDemandFontMenuItem( fontSizeMenu, 0,
                        lString16(defvalue),
                        LVImageSourceRef(), lString16::itoa(fontSizes[i]).c_str(),
                        fontSizes[i], 400, false, css_ff_sans_serif, fontFace, UnicodeToUtf8(skin->getItemSkin()->getFontFace())) );
    }
    fontSizeMenu->setAccelerators( _wm->getAccTables().get("menu") );
    //fontSizeMenu->setAccelerators( _menuAccelerators );
    fontSizeMenu->setSkinName(lString16("#settings"));
    //fontSizeMenu->setSkinName(lString16("#main"));
    fontSizeMenu->reconfigure( 0 );
    if(NULL != mainMenu)
        mainMenu->addItem(fontSizeMenu);
    return fontSizeMenu;
}

void CRSettingsMenu::addMenuItems( CRMenu * menu, item_def_t values[] )
{
    for ( int i=0; values[i].translate_default; i++)
        menu->addItem( new CRMenuItem( menu, i,
            lString16(values[i].translate_default),
            LVImageSourceRef(),
            LVFontRef(), Utf8ToUnicode(lString8(values[i].value)).c_str() ) );
    menu->setAccelerators( _menuAccelerators );
    menu->setSkinName(lString16("#settings"));
    menu->reconfigure( 0 );
}

//This is to avoid toggleSubmenuValue() when number of items <= 3 in CRMenu,
//because it doesn't work well when style value is empty (inherited)
class CRStyleMenu : public CRMenu
{
public:
    CRStyleMenu( CRGUIWindowManager * wm, CRMenu * parentMenu, int id, const char * label, LVFontRef valueFont, CRPropRef props, const char * propName=NULL)
        : CRMenu(wm, parentMenu, id, label, LVImageSourceRef(), LVFontRef(), valueFont, props, propName)
    {
    }

    int onSelect()
    {
        if (isSubmenu()) {
            // show menu
            _wm->activateWindow( (CRMenu *)this );
            return 1;
        }
        return 0;
    }

#if 0
    void toggleSubmenuValue()
    {
        if ( !isSubmenu() || _propName.empty() || _props.isNull() )
            return;
        lString16 value = getProps()->getStringDef(
                                   UnicodeToUtf8(getPropName()).c_str(), "");
        for ( int i=0; i<_items.length(); i++ ) {
            if ( value==(_items[i]->getPropValue()) ) {
                int n = (i + 1) % _items.length();
                getProps()->setString(UnicodeToUtf8(getPropName()).c_str(), _items[n]->getPropValue() );
                return;
            }
        }
    }
#endif

    bool onItemSelect(int itemId, int param = 0 )
    {
        if (itemId < 0 || itemId >= _items.length()) {
                CRLog::error( "CRMenu::onItemSelect() - invalid selection: %d", itemId);
                return true;
        }

        CRMenuItem * item = _items[itemId];

        if (item->onSelect() > 0)
            return true;

        if (item->isSubmenu()) {
            // show menu
            _wm->activateWindow( (CRMenu *)item );
        } else {
            if ( !getPropName().empty() ) {
                // set property
                _props->setString( UnicodeToUtf8(getPropName()).c_str(), item->getPropValue() );
                doCloseMenu(getId());
            } else
                // command menu item
                doCloseMenu(item->getId(), true, param);
        }
        return true;
    }
};

class CRSkinSelectMenu : public CRMenu
{
public:
    CRSkinSelectMenu( CRGUIWindowManager * wm, CRMenu * parentMenu, int id, const char * label, LVFontRef valueFont, CRPropRef props)
        : CRMenu(wm, parentMenu, id, label, LVImageSourceRef(), LVFontRef(), valueFont, props, PROP_SKIN_FILE)
    {
    }

    int onSelect()
    {
        if (isSubmenu()) {
            // show menu
            _wm->activateWindow( (CRMenu *)this );
            return 1;
        }
        return 0;
    }

    bool onItemSelect(int itemId, int param = 0 )
    {
        if (itemId < 0 || itemId >= _items.length()) {
                CRLog::error( "CRMenu::onItemSelect() - invalid selection: %d", itemId);
                return true;
        }

        CRMenuItem * item = _items[itemId];

        if (item->onSelect() > 0)
            return true;

        if (item->isSubmenu()) {
            // show menu
            _wm->activateWindow( (CRMenu *)item );
        } else {
            // set property
            if (_wm->setSkin(item->getPropValue()))
                _props->setString( UnicodeToUtf8(getPropName()).c_str(), item->getPropValue() );
            doCloseMenu(getId());
        }
        return true;
    }
};

CRMenu * CRSettingsMenu::createFontFaceMenuItem( CRMenu * mainMenu, LVFontRef valueFont, int id, const char * label, const char * propName)
{
    CRMenu * fontFaceMenu = new CRMenu(_wm, mainMenu, id,
                                       label,
                                       LVImageSourceRef(), LVFontRef(), valueFont, props, propName );
    fontFaceMenu->setSkinName(lString16("#settings"));
    CRLog::trace("getting font face list");
    lString16Collection list;
    fontMan->getFaceList( list );
    CRLog::trace("faces found: %d", list.length());
    int i;
    lString8 defFont = UnicodeToUtf8(getSkin()->getItemSkin()->getFontFace());
    for ( i=0; i<(int)list.length(); i++ ) {
        fontFaceMenu->addItem( new OnDemandFontMenuItem( fontFaceMenu, i,
                                                        list[i], LVImageSourceRef(), list[i].c_str(),
                                                        MENU_FONT_FACE_SIZE, 400,
                                                        false, css_ff_sans_serif, UnicodeToUtf8(list[i]), defFont) );
    }
    fontFaceMenu->setFullscreen( true );
    fontFaceMenu->setAccelerators( _menuAccelerators );
    fontFaceMenu->reconfigure( 0 );
    mainMenu->addItem(fontFaceMenu);
    return fontFaceMenu;
}

CRMenu * CRSettingsMenu::createStyleMenuItem(CRMenu * menu, LVFontRef valueFont, const char * label, lString8 property, item_def_t values[])
{
    CRMenu * menuItem;
    menuItem = new CRStyleMenu(_wm, menu, _menuItemId++,
                               label, valueFont, props,  property.c_str());
    addMenuItems(menuItem, values);
    menu->addItem( menuItem );
    return menuItem;
}

void CRSettingsMenu::createStyleMenuItems(CRMenu * menu, LVFontRef valueFont, const char *prefix )
{
    item_def_t css_align_options[] = {
        {"-", ""}, // inherited
        {_("Justify"), "text-align: justify"},
        {C_("CSS", "Left"), "text-align: left"},
        {_("Center"), "text-align: center"},
        {C_("CSS", "Right"), "text-align: right"},
        {NULL, NULL}
    };

    item_def_t ident_options[] = {
        {"-", ""}, // inherited
        {_("No indent"), "text-indent: 0em"},
        {_("Small indent"), "text-indent: 1.2em"},
        {_("Big indent"), "text-indent: 2em"},
        {_("Small outdent"), "text-indent: -1.2em"},
        {_("Big outdent"), "text-indent: -2em"},
        {NULL, NULL}
    };

    item_def_t font_face_options[] = {
        {"-", ""}, // inherited
        {_("[Default sans-serif font]"), "font-family: sans-serif"},
        {_("[Default serif font]"), "font-family: serif"},
        {_("[Default monospace font]"), "font-family: \"Courier New\", \"Courier\", monospace"},
        {NULL, NULL}
    };

    item_def_t font_size_options[] = {
        {"-", ""}, // inherited
        {_("Bigger (105%)"), "font-size: 105%"},
        {_("Bigger (110%)"), "font-size: 110%"},
        {_("Bigger (115%)"), "font-size: 115%"},
        {_("Bigger (120%)"), "font-size: 120%"},
        {_("Bigger (150%)"), "font-size: 150%"},
        {_("Smaller (95%)"), "font-size: 95%"},
        {_("Smaller (90%)"), "font-size: 90%"},
        {_("Smaller (85%)"), "font-size: 85%"},
        {_("Smaller (80%)"), "font-size: 80%"},
        {_("Smaller (70%)"), "font-size: 70%"},
        {_("Smaller (60%)"), "font-size: 60%"},
        {NULL, NULL}
    };

    item_def_t font_weight_options[] = {
        {"-", ""}, // inherited
        {_("Normal"), "font-weight: normal"},
        {_("Bold"), "font-weight: bold"},
        {_("Bolder"), "font-weight: bolder"},
        {_("Lighter"), "font-weight: lighter"},
        {NULL, NULL}
    };

    item_def_t font_style_options[] = {
        {"-", ""}, // inherited
        {_("Normal"), "font-style: normal"},
        {_("Italic"), "font-style: italic"},
        {NULL, NULL}
    };

    item_def_t line_height_options[] = {
        {"-", ""}, // inherited
        {"75%", "line-height: 75%"},
        {"80%", "line-height: 80%"},
        {"85%", "line-height: 85%"},
        {"90%", "line-height: 90%"},
        {"95%", "line-height: 95%"},
        {"100%", "line-height: 100%"},
        {"105%", "line-height: 105%"},
        {"110%", "line-height: 110%"},
        {"115%", "line-height: 115%"},
        {"120%", "line-height: 120%"},
        {"130%", "line-height: 130%"},
        {"140%", "line-height: 140%"},
        {"150%", "line-height: 150%"},
        {NULL, NULL}
    };

    item_def_t text_decoration_options[] = {
        {"-", ""}, // inherited
        {_("None"), "text-decoration: none"},
        {_("Underline"), "text-decoration: underline"},
        {_("Line through"), "text-decoration: line-through"},
        {_("Overline"), "text-decoration: overline"},
        {NULL, NULL}
    };

    item_def_t vertical_align_options[] = {
        {"-", ""}, // inherited
        {_("Baseline"), "vertical-align: baseline"},
        {_("Subscript"), "vertical-align: sub"},
        {_("Superscript"), "vertical-align: super"},
        {NULL, NULL}
    };

    item_def_t font_color_options[] = {
        {"-", ""}, // inherited
        {_("Black"), "color: black"},
        {_("Gray"), "color: gray"},
        {_("White"), "color: white"},
        {NULL, NULL}
    };

    item_def_t background_color_options[] = {
        {"-", ""}, // inherited
        {_("Black"), "background-color: black"},
        {_("Gray"), "background-color: gray"},
        {_("White"), "background-color: white"},
        {NULL, NULL}
    };

    item_def_t margin_top_options[] = {
        {"-", ""}, // inherited
        {"0", "margin-top: 0em"},
        {_("0.2 of line height"), "margin-top: 0.2em"},
        {_("0.3 of line height"), "margin-top: 0.3em"},
        {_("0.5 of line height"), "margin-top: 0.5em"},
        {_("1 line height"), "margin-top: 1em"},
        {_("2 of line height"), "margin-top: 2em"},
        {NULL, NULL}
    };

    item_def_t margin_bottom_options[] = {
        {"-", ""}, // inherited
        {"0", "margin-bottom: 0em"},
        {_("0.2 of line height"), "margin-bottom: 0.2em"},
        {_("0.3 of line height"), "margin-bottom: 0.3em"},
        {_("0.5 of line height"), "margin-bottom: 0.5em"},
        {_("1 line height"), "margin-bottom: 1em"},
        {_("2 of line height"), "margin-bottom: 2em"},
        {NULL, NULL}
    };

    item_def_t margin_left_options[] = {
        {"-", ""}, // inherited
        {"0", "margin-left: 0em"},
        {_("0.5 of line height"), "margin-left: 0.5em"},
        {_("1 line height"), "margin-left: 1em"},
        {_("1.5 of line height"), "margin-left: 1.5em"},
        {_("2 of line height"), "margin-left: 2em"},
        {_("4 of line height"), "margin-left: 4em"},
        {_("5% of line width"), "margin-left: 5%"},
        {_("10% of line width"), "margin-left: 10%"},
        {_("15% of line width"), "margin-left: 15%"},
        {_("20% of line width"), "margin-left: 20%"},
        {_("30% of line width"), "margin-left: 30%"},
        {NULL, NULL}
    };

    item_def_t margin_right_options[] = {
        {"-", ""}, // inherited
        {"0", "margin-right: 0em"},
        {_("0.5 of line height"), "margin-right: 0.5em"},
        {_("1 line height"), "margin-right: 1em"},
        {_("1.5 of line height"), "margin-right: 1.5em"},
        {_("2 of line height"), "margin-right: 2em"},
        {_("4 of line height"), "margin-right: 4em"},
        {_("5% of line width"), "margin-right: 5%"},
        {_("10% of line width"), "margin-right: 10%"},
        {_("15% of line width"), "margin-right: 15%"},
        {_("20% of line width"), "margin-right: 20%"},
        {_("30% of line width"), "margin-right: 30%"},
        {NULL, NULL}
    };
    createStyleMenuItem(menu, valueFont, _("Text alignment"),
                    lString8(prefix) + ".align", css_align_options);
    createStyleMenuItem(menu, valueFont, _("First line indent"),
                    lString8(prefix) + ".text-indent", ident_options);

    CRMenu * menuItem = createStyleMenuItem(menu, valueFont, _("Font face"),
                                        lString8(prefix) + ".font-face", font_face_options);
    lString16Collection list;
    fontMan->getFaceList( list );
    lString8 defFont = UnicodeToUtf8(getSkin()->getItemSkin()->getFontFace());

    for ( int i=0; i<(int)list.length(); i++ ) {
        lString16 fontFamily = lString16("font-family: ") + list[i];
        menuItem->addItem( new OnDemandFontMenuItem( menuItem, i,
                                                    list[i], LVImageSourceRef(), fontFamily.c_str(),
                                                    MENU_FONT_FACE_SIZE, 400,
                                                    false, css_ff_sans_serif, UnicodeToUtf8(list[i]), defFont) );
    }

    createStyleMenuItem(menu, valueFont, _("Font size"),
                    lString8(prefix) + ".font-size", font_size_options);
    createStyleMenuItem(menu, valueFont, _("Font weight"),
                    lString8(prefix) + ".font-weight", font_weight_options);
    createStyleMenuItem(menu, valueFont, _("Font style"),
                    lString8(prefix) + ".font-style", font_style_options);
    createStyleMenuItem(menu, valueFont, _("Interline space"),
                    lString8(prefix) + ".line-height", line_height_options);
    createStyleMenuItem(menu, valueFont, _("Text decoration"),
                    lString8(prefix) + ".text-decoration", text_decoration_options);
    createStyleMenuItem(menu, valueFont, _("Text vertical alignment"),
                    lString8(prefix) + ".vertical-align", vertical_align_options);
    createStyleMenuItem(menu, valueFont, _("Text color"),
                    lString8(prefix) + ".color", font_color_options);
    createStyleMenuItem(menu, valueFont, _("Background color"),
                    lString8(prefix) + ".background-color", background_color_options);
    createStyleMenuItem(menu, valueFont, _("Margin before"),
                    lString8(prefix) + ".margin-top", margin_top_options);
    createStyleMenuItem(menu, valueFont, _("Margin after"),
                    lString8(prefix) + ".margin-bottom", margin_bottom_options);
    createStyleMenuItem(menu, valueFont, _("Margin left"),
                    lString8(prefix) + ".margin-left",  margin_left_options);
    createStyleMenuItem(menu, valueFont, _("Margin right"),
                    lString8(prefix) + ".margin-right", margin_right_options);
}

CRMenu * CRSettingsMenu::createSettingsMenuItem(CRMenu * menu, int id, const char * label, LVFontRef valueFont, const char * propName, item_def_t values[])
{
    CRMenu * menuItem;
    menuItem = new CRMenu(_wm, menu, id, label,
                          LVImageSourceRef(), LVFontRef(), valueFont, props, propName);
    addMenuItems(menuItem, values);
    menu->addItem( menuItem );
    return menuItem;
}

CRSettingsMenu::CRSettingsMenu( CRGUIWindowManager * wm, CRPropRef newProps, int id, LVFontRef font, CRGUIAcceleratorTableRef menuAccelerators, lvRect &rc )
: CRFullScreenMenu( wm, id, lString16(_("Settings")), 8, rc ),
  props( newProps ),
  _menuAccelerators( menuAccelerators ), _menuItemId(mm_Last)
{
    if( pbSkinFileName == lString16("pb626fw5.cr3skin") )
        forcePartialBwUpdates = true;

    setSkinName(lString16("#settings"));

    _fullscreen = true;

    item_def_t antialiasing_modes[] = {
        {_("On for all fonts"), "2"},
        {_("On for big fonts only"), "1"},
        {_("Off"), "0"},
        {NULL, NULL},
    };

    item_def_t on_off_option[] = {
        {_("On"), "1"},
        {_("Off"), "0"},
        {NULL, NULL},
    };

    item_def_t highlight_bookmark[] = {
        {_("None"), "0"},
        {_("Solid"), "1"},
        {_("Underline"), "2"},
        {NULL, NULL},
    };

    item_def_t space_condensing_percent[] = {
        {"25%", "25"},
        {"30%", "30"},
        {"35%", "35"},
        {"40%", "40"},
        {"45%", "45"},
        {"50%", "50"},
        {"55%", "55"},
        {"60%", "60"},
        {"65%", "65"},
        {"70%", "70"},
        {"75%", "75"},
        {"80%", "80"},
        {"85%", "85"},
        {"90%", "90"},
        {"95%", "95"},
        {"100%", "100"},
        {NULL, NULL}
    };

    item_def_t inverse_mode[] = {
        {_("Normal"), "0"},
        {_("Inverse"), "1"},
        {NULL, NULL},
    };

    item_def_t embolden_mode[] = {
        {_("Normal"), "0"},
        {_("Bold"), "1"},
        {NULL, NULL},
    };

    item_def_t landscape_pages[] = {
        {_("One"), "1"},
        {_("Two"), "2"},
        {NULL, NULL},
    };

    item_def_t status_line[] = {
        {_("Top"), "0"},
	//    {"VIEWER_DLG_STATUS_LINE_BOTTOM", "Bottom", "1"},
        {_("Off"), "2"},
        {NULL, NULL},
    };

    item_def_t interline_spaces[] = {
        {_("70%"), "70"},
        {_("75%"), "75"},
        {_("80%"), "80"},
        {_("85%"), "85"},
        {_("90%"), "90"},
        {_("95%"), "95"},
        {_("100%"), "100"},
        {_("105%"), "105"},
        {_("110%"), "110"},
        {_("115%"), "115"},
        {_("120%"), "120"},
        {_("125%"), "125"},
        {_("130%"), "130"},
        {_("135%"), "135"},
        {_("140%"), "140"},
        {_("145%"), "145"},
        {_("150%"), "150"},
        {_("160%"), "160"},
        {_("180%"), "180"},
        {_("200%"), "200"},
        {NULL, NULL},
    };

    item_def_t page_margins[] = {
    #if BIG_PAGE_MARGINS==1
        //static int def_margin[] = { 8, 0, 5, 10, 20, 30, 50, 60 };
        {"0", "0"},
        {"1", "1"},
        {"2", "2"},
        {"3", "3"},
        {"4", "4"},
        {"5", "5"},
        {"8", "8"},
        {"10", "10"},
        {"12", "12"},
        {"14", "14"},
        {"16", "16"},
        {"20", "20"},
        {"22", "22"},
        {"24", "24"},
        {"26", "26"},
        {"30", "30"},
        {"50", "50"},
        {"60", "60"},
        {"80", "80"},
        {"100", "100"},
        {"130", "130"},
        {"200", "200"},
        {"300", "300"},
    #else
        {"0", "0"},
        {"5", "5"},
        {"8", "8"},
        {"10", "10"},
        {"15", "15"},
        {"20", "20"},
        {"25", "25"},
        {"30", "30"},
    #endif
        {NULL, NULL},
    };
#if ENABLE_UPDATE_MODE_SETTING==1
    item_def_t screen_update_options[] = {
        {_("Always use fast updates"), "0"},
        {_("Don't use fast updates"), "1"},
        {_("Full updates every 2 pages"), "2"},
        {_("Full updates every 3 pages"), "3"},
        {_("Full updates every 4 pages"), "4"},
        {_("Full updates every 5 pages"), "5"},
        {_("Full updates every 6 pages"), "6"},
        {_("Full updates every 7 pages"), "7"},
        {_("Full updates every 8 pages"), "8"},
        {_("Full updates every 9 pages"), "9"},
        {_("Full updates every 10 pages"), "10"},
        {_("Full updates every 14 pages"), "14"},
        {NULL, NULL},
    };
#endif
    item_def_t turbo_update_options[] = {
        {_("Turbo mode disabled"), "0"},
        {_("Turbo mode enabled"), "1"},
        {NULL, NULL},
    };
#ifdef CR_POCKETBOOK
    item_def_t rotate_mode_options[] = {
        {"360°", "0"},
        {"180°", "1"},
        {_("180+slow next page"), "2"},
        {_("180+slow prev/next page"), "3"},
        {_("180+FAST next page"), "4"},
        {_("180+FAST prev/next page"), "5"},
        {_("180+FAST next/prev page"), "6"},
        {NULL, NULL}
    };
    item_def_t rotate_angle_options[] = {
        {"20°", "0"},
        {"25°", "1"},
        {"30°", "2"},
        {"35°", "3"},
        {"40°", "4"},
        {"45°", "5"},
        {"50°", "6"},
        {"55°", "7"},
        {"60°", "8"},
        {"65°", "9"},
        {"70°", "10"},
        {NULL, NULL}
    };
    item_def_t gray_buffer_bpp[] = {
        {_("2 BPP"), "2"},
        {_("3 BPP"), "3"},
        {_("4 BPP"), "4"},
        {_("8 BPP"), "8"},
        {NULL, NULL}
    };
#endif

    item_def_t image_scaling_modes[] = {
        {_("Disabled (1:1)"), "0"},
        {_("Integer scale"), "1"},
        {_("Arbitrary scale"), "2"},
        {NULL, NULL}
    };

    item_def_t image_scaling_factors[] = {
        {_("Auto"), "0"},
        {_("*1"), "1"},
        {_("*2"), "2"},
        {_("*3"), "3"},
        {NULL, NULL}
    };

    item_def_t font_gammas[] = {
        {"0.3",  "0.3"},
        {"0.35", "0.35"},
        {"0.4",  "0.4"},
        {"0.45", "0.45"},
        {"0.5",  "0.5"},
        {"0.55", "0.55"},
        {"0.6",  "0.6"},
        {"0.65", "0.65"},
        {"0.7",  "0.7"},
        {"0.75", "0.75"},
        {"0.8",  "0.8"},
        {"0.85", "0.85"},
        {"0.9",  "0.9"},
        {"0.95", "0.95"},
        {"1.0",  "1.0"},
        {"1.05", "1.05"},
        {"1.1",  "1.1"},
        {"1.15", "1.15"},
        {"1.2",  "1.2"},
        {"1.25", "1.25"},
        {"1.3",  "1.3"},
        {"1.35", "1.35"},
        {"1.4",  "1.4"},
        {"1.45", "1.45"},
        {"1.5",  "1.5"},
        {"1.55", "1.55"},
        {"1.6",  "1.6"},
        {"1.65", "1.65"},
        {"1.7",  "1.7"},
        {"1.75", "1.75"},
        {"1.8",  "1.8"},
        {"1.85", "1.85"},
        {"1.9",  "1.9"},
        {"1.95", "1.95"},
        {"2.0",  "2.0"},
        {"2.5",  "2.5"},
        {"3.0",  "3.0"},
        {"4.0",  "4.0"},
        {"5.0",  "5.0"},
        {"7.0",  "7.0"},
        {"11.0", "11.0"},
        {NULL, NULL}
    };

    item_def_t font_hinting[] = {
        {_("No hinting"), "0"},
        {_("Use bytecode"), "1"},
        {_("Autohinting"), "2"},
        {_("Light"), "3"},
        {NULL, NULL}
    };

    item_def_t css_styles[] = {
        {_("Default paragraph formatting"), "styles.def"},
        {C_("CSS", "Title"), "styles.title"},
        {_("Subtitle"), "styles.subtitle"},
        {_("Preformatted text / code"), "styles.pre"},
        {_("Link"), "styles.link"},
        {_("Cite"), "styles.cite"},
        {_("Epigraph"), "styles.epigraph"},
        {_("Poem"), "styles.poem"},
        {_("Text / Cite / Epigraph author"), "styles.text-author"},
        {_("Footnote"), "styles.footnote"},
        {_("Footnote link"), "styles.footnote-link"},
        {_("Footnote title"), "styles.footnote-title"},
        {_("Annotation"), "styles.annotation"},
        {NULL, NULL}
    };

    item_def_t tap_zone_sizes[] = {
        {"20%",  "20"},
        {"25%",  "25"},
        {"30%",  "30"},
        {"33%",  "33"},
        {"40%",  "40"},
        {"50%",  "50"},
        {"60%",  "60"},
        {NULL, NULL}
    };

    CRLog::trace("showSettingsMenu() - %d property values found", props->getCount() );

    setSkinName(lString16("#settings"));
    //setSkinName(lString16(L"#main"));

    LVFontRef valueFont( fontMan->GetFont( VALUE_FONT_SIZE, 400, true, css_ff_sans_serif, lString8("Arial")) );
    CRMenu * mainMenu = this;
    mainMenu->setAccelerators( _menuAccelerators );

    createFontFaceMenuItem(mainMenu, valueFont, mm_FontFace, _("Default font face"),  PROP_FONT_FACE);
    createFontSizeMenu( mainMenu, cr_font_sizes, sizeof(cr_font_sizes)/sizeof(int), props, PROP_FONT_SIZE);

    CRMenu * fontSettingsMenu = new CRMenu( _wm, mainMenu, _menuItemId++, _("Font settings"),
                                           LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef());
    fontSettingsMenu->setSkinName(lString16("#settings"));
    fontSettingsMenu->setAccelerators( _menuAccelerators );

    createSettingsMenuItem(fontSettingsMenu, mm_Embolden, _("Font weight"),
                           valueFont, PROP_FONT_WEIGHT_EMBOLDEN, embolden_mode);
    createSettingsMenuItem(fontSettingsMenu, mm_FontAntiAliasing, _("Font antialiasing"),
                           valueFont, PROP_FONT_ANTIALIASING, antialiasing_modes);
    createSettingsMenuItem(fontSettingsMenu, mm_FontHinting, _("Font hinting"),
                           valueFont, PROP_FONT_HINTING, font_hinting);
    createSettingsMenuItem(fontSettingsMenu, mm_Kerning, _("Font kerning"),
                           valueFont, PROP_FONT_KERNING_ENABLED, on_off_option);
    createSettingsMenuItem(fontSettingsMenu, mm_fontGamma, _("Font Gamma"),
                           valueFont, PROP_FONT_GAMMA, font_gammas);
    createSettingsMenuItem(fontSettingsMenu, mm_EmbeddedFonts, _("Document embedded fonts"),
                           valueFont, PROP_EMBEDDED_FONTS, on_off_option);
    createFontFaceMenuItem(fontSettingsMenu, valueFont, mm_FontFallbackFace, _("Fallback font face"),
                           PROP_FALLBACK_FONT_FACE);

    fontSettingsMenu->reconfigure( 0 );
    mainMenu->addItem( fontSettingsMenu );

    CRMenu * formatSettingsMenu = new CRMenu( _wm, mainMenu, _menuItemId++, _("Formatting settings"),
                                             LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef());
    formatSettingsMenu->setSkinName(lString16("#settings"));
    formatSettingsMenu->setAccelerators( _menuAccelerators );

    createSettingsMenuItem(formatSettingsMenu, mm_InterlineSpace, _("Interline space"),
                           valueFont, PROP_INTERLINE_SPACE, interline_spaces);

    //====== Hyphenation ==========
    if ( HyphMan::getDictList() ) {
        // strings from CREngine - just to catch by gettext tools
        _("[No Hyphenation]");
        _("[Algorythmic Hyphenation]");
        CRMenu * hyphMenu = new CRMenu(_wm, formatSettingsMenu, mm_Hyphenation,
                                       _("Hyphenation"),
                                       LVImageSourceRef(), LVFontRef(), valueFont, props, PROP_HYPHENATION_DICT );
        for ( int i=0; i<HyphMan::getDictList()->length(); i++ ) {
            HyphDictionary * item = HyphMan::getDictList()->get( i );
            hyphMenu->addItem( new CRMenuItem( hyphMenu, i,
                                              item->getTitle(),
                                              LVImageSourceRef(),
                                              LVFontRef(), item->getId().c_str() ) );
        }
        hyphMenu->setAccelerators( _menuAccelerators );
        hyphMenu->setSkinName(lString16("#settings"));
        hyphMenu->reconfigure( 0 );
        formatSettingsMenu->addItem( hyphMenu );
    }
    createSettingsMenuItem(formatSettingsMenu, mm_PreformattedText, _("Preformatted text"),
                           valueFont, PROP_TXT_OPTION_PREFORMATTED, on_off_option);
    createSettingsMenuItem(formatSettingsMenu, mm_SpaceCondensing, _("Space condensing percent"),
                           valueFont, PROP_FORMAT_MIN_SPACE_CONDENSING_PERCENT, space_condensing_percent);
    createSettingsMenuItem(formatSettingsMenu, mm_FloatingPunctuation, _("Floating punctuation"),
                           valueFont, PROP_FLOATING_PUNCTUATION, on_off_option);

    //====== Image scaling ==============
    CRMenu * scalingMenu = new CRMenu(_wm, formatSettingsMenu, mm_ImageScaling,
                                      _("Image scaling"), LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef() );
    scalingMenu->setAccelerators( _menuAccelerators );
    scalingMenu->setSkinName(lString16("#settings"));

    createSettingsMenuItem(scalingMenu, mm_blockImagesZoominMode, _("Block image scaling mode"),
                           valueFont, PROP_IMG_SCALING_ZOOMIN_BLOCK_MODE, image_scaling_modes);
    createSettingsMenuItem(scalingMenu, mm_blockImagesZoominScale, _("Block image max zoom"),
                           valueFont, PROP_IMG_SCALING_ZOOMIN_BLOCK_SCALE, image_scaling_factors);
    createSettingsMenuItem(scalingMenu, mm_inlineImagesZoominMode, _("Inline image scaling mode"),
                           valueFont, PROP_IMG_SCALING_ZOOMIN_INLINE_MODE, image_scaling_modes);
    createSettingsMenuItem(scalingMenu, mm_inlineImagesZoominScale, _("Inline image max zoom"),
                           valueFont, PROP_IMG_SCALING_ZOOMIN_INLINE_SCALE, image_scaling_factors);

    scalingMenu->reconfigure( 0 );
    formatSettingsMenu->addItem( scalingMenu );
    formatSettingsMenu->reconfigure( 0 );
    mainMenu->addItem( formatSettingsMenu );

    CRMenu * pageSettingsMenu = new CRMenu( _wm, mainMenu, _menuItemId++, _("Page settings"),
                                           LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef());
    pageSettingsMenu->setSkinName(lString16("#settings"));
    pageSettingsMenu->setAccelerators( _menuAccelerators );

    createSettingsMenuItem(pageSettingsMenu, mm_StatusLine, _("Status line"),
                           valueFont, PROP_STATUS_LINE, status_line);

    //====== Status line settings ========
    CRMenu * statusMenu = new CRMenu(_wm, pageSettingsMenu, mm_StatusMenu,
                                     _("Status line settings"), LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef() );
    statusMenu->setAccelerators( _menuAccelerators );
    statusMenu->setSkinName(lString16(L"#settings"));

    createFontFaceMenuItem(statusMenu, valueFont, _menuItemId++, _("Font face"),
                           PROP_STATUS_FONT_FACE);

    unsigned sizesCount = sizeof(cr_status_font_sizes)/sizeof(cr_status_font_sizes[0]);
    for(unsigned i = 0; i < sizesCount; i++) {
        cr_status_font_sizes[i] = MIN_STATUS_FONT_SIZE + i;
    }
    createFontSizeMenu(statusMenu, cr_status_font_sizes, sizesCount, props, PROP_STATUS_FONT_SIZE);
    createSettingsMenuItem(statusMenu, mm_StatusFontEmbolden, _("Font weight"),
                           valueFont, PROP_STATUS_FONT_EMBOLDEN, embolden_mode);
    createSettingsMenuItem(statusMenu, mm_ShowTime, _("Show time"),
                           valueFont, PROP_SHOW_TIME, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowTitle, _("Show title"),
                           valueFont, PROP_SHOW_TITLE, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowBattery, _("Show battery"),
                           valueFont, PROP_SHOW_BATTERY, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowBatteryPercent, _("Show battery percent"),
                           valueFont, PROP_SHOW_BATTERY_PERCENT, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowPosPercent, _("Show position percent"),
                           valueFont, PROP_SHOW_POS_PERCENT, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowPageCount, _("Show page count"),
                           valueFont, PROP_SHOW_PAGE_COUNT, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowPagesTillChapterEnd, _("Show page count till chapter end"),
                           valueFont, PROP_SHOW_CHAPTER_PAGES_REMAIN, on_off_option);
    createSettingsMenuItem(statusMenu, mm_ShowPageNumber, _("Show page number"),
                           valueFont, PROP_SHOW_PAGE_NUMBER, on_off_option);
    createSettingsMenuItem(statusMenu, mm_showChapterMarks, _("Show chapter marks"),
                           valueFont, PROP_STATUS_CHAPTER_MARKS, on_off_option);
    statusMenu->reconfigure( 0 );
    pageSettingsMenu->addItem( statusMenu );
    //====== End of Status line  ========

    //====== Margins ==============
    CRMenu * marginsMenu = new CRMenu(_wm, pageSettingsMenu, mm_PageMargins,
                                      _("Page margins"),
                                      LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef() );
    marginsMenu->setAccelerators( _menuAccelerators );
    marginsMenu->setSkinName(lString16("#settings"));

    createSettingsMenuItem(marginsMenu, mm_PageMarginTop, _("Top margin"),
                           valueFont, PROP_PAGE_MARGIN_TOP, page_margins);
    createSettingsMenuItem(marginsMenu, mm_PageMarginBottom, _("Bottom margin"),
                           valueFont, PROP_PAGE_MARGIN_BOTTOM, page_margins);
    createSettingsMenuItem(marginsMenu, mm_PageMarginLeft, _("Left margin"),
                           valueFont, PROP_PAGE_MARGIN_LEFT, page_margins);
    createSettingsMenuItem(marginsMenu, mm_PageMarginRight, _("Right margin"),
                           valueFont, PROP_PAGE_MARGIN_RIGHT, page_margins);
    marginsMenu->reconfigure( 0 );
    pageSettingsMenu->addItem( marginsMenu );
    //====== End of Margins ==============
    createSettingsMenuItem(pageSettingsMenu, mm_Footnotes, _("Footnotes at page bottom"),
                           valueFont, PROP_FOOTNOTES, on_off_option);

    pageSettingsMenu->reconfigure( 0 );
    mainMenu->addItem( pageSettingsMenu );

    CRMenu * displaySettingsMenu = new CRMenu( _wm, mainMenu, _menuItemId++, _("Display settings"),
                                              LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef());
    displaySettingsMenu->setSkinName(lString16("#settings"));
    displaySettingsMenu->setAccelerators( _menuAccelerators );

#if CR_INTERNAL_PAGE_ORIENTATION==1 || defined(CR_POCKETBOOK)
    CRMenu * orientationMenu = createOrientationMenu(displaySettingsMenu, props);
    displaySettingsMenu->addItem( orientationMenu );
#endif
    createSettingsMenuItem(displaySettingsMenu, mm_LandscapePages, _("Landscape pages"),
                           valueFont, PROP_LANDSCAPE_PAGES, landscape_pages);
    createSettingsMenuItem(displaySettingsMenu, mm_Inverse, _("Inverse display"),
                           valueFont, PROP_DISPLAY_INVERSE, inverse_mode);
#if 0
    createSettingsMenuItem(displaySettingsMenu, mm_BookmarkIcons, _("Show bookmark icons"),
                           valueFont, PROP_BOOKMARK_ICONS, bookmark_icons);
#endif
    createSettingsMenuItem(displaySettingsMenu, mm_HighlightBookmarks, _("Highlight bookmarks"),
                           valueFont, PROP_HIGHLIGHT_COMMENT_BOOKMARKS, highlight_bookmark);
#if ENABLE_UPDATE_MODE_SETTING==1
    createSettingsMenuItem(displaySettingsMenu, mm_FastUpdates, _("Display update mode"),
                           valueFont, PROP_DISPLAY_FULL_UPDATE_INTERVAL, screen_update_options);
#endif
    if ( _wm->getScreen()->getTurboUpdateSupported() ) {
        createSettingsMenuItem(displaySettingsMenu, mm_TurboUpdateMode, _("Turbo update mode"),
                               valueFont, PROP_DISPLAY_TURBO_UPDATE_MODE, turbo_update_options);
    }
#ifdef CR_POCKETBOOK
    createSettingsMenuItem(displaySettingsMenu, mm_grayBufferMode, _("Gray buffer depth(need restart)"),
                           valueFont, PROP_POCKETBOOK_GRAYBUFFER_BPP, gray_buffer_bpp);
#endif
    CRSkinList &skins = wm->getSkinList();
    if (skins.length() > 1) {
        CRSkinSelectMenu * skinsMenu = new CRSkinSelectMenu(_wm, displaySettingsMenu, mm_Skin,
                                       _("Skin"), valueFont, props);
        for ( int i=0; i<skins.length(); i++ ) {
            CRSkinListItem * item = skins.get( i );
            skinsMenu->addItem( new CRMenuItem( skinsMenu, i,
                                              item->getTitle(),
                                              LVImageSourceRef(),
                                              LVFontRef(), item->getId().c_str() ) );
        }
        skinsMenu->setAccelerators( _menuAccelerators );
        skinsMenu->setSkinName(lString16("#settings"));
        skinsMenu->reconfigure( 0 );
        displaySettingsMenu->addItem( skinsMenu );
    }
    displaySettingsMenu->reconfigure( 0 );
    mainMenu->addItem( displaySettingsMenu );

    CRMenu * controlSettingsMenu = new CRStyleMenu( _wm, mainMenu, _menuItemId++, _("Control settings"),
                                              valueFont, CRPropRef());
    //style menu doesn't toggle when number of items <= 3, we don't need toggling
    controlSettingsMenu->setSkinName(lString16("#settings"));
    controlSettingsMenu->setAccelerators( _menuAccelerators );
#ifdef CR_POCKETBOOK
    if (isGSensorSupported()) {
        createSettingsMenuItem(controlSettingsMenu, mm_rotateMode, _("Rotate"),
                               valueFont, PROP_POCKETBOOK_ROTATE_MODE, rotate_mode_options);
        createSettingsMenuItem(controlSettingsMenu, mm_rotateAngle, _("Page turn angle"),
                               valueFont, PROP_POCKETBOOK_ROTATE_ANGLE, rotate_angle_options);
    }
#endif
#ifndef CR_POCKETBOOK
    CRControlsMenu * controlsMenu =
            new CRControlsMenu(controlSettingsMenu, mm_Controls, props, lString16("main"), 8, _rect);
    controlsMenu->setAccelerators( _menuAccelerators );
    controlsMenu->setSkinName(lString16("#settings"));
    controlsMenu->setValueFont(valueFont);
    controlsMenu->reconfigure( 0 );
    controlSettingsMenu->addItem( controlsMenu );
#endif
    if (getWindowManager()->getScreen()->isTouchSupported()) {
        CRTapZoneSettingsMenuItem * touchMenu = new CRTapZoneSettingsMenuItem(controlSettingsMenu, props,
                                                                              _("Tap action"), PROP_TAP_ZONE_ACTION);
        controlSettingsMenu->addItem(touchMenu);
        touchMenu = new CRTapZoneSettingsMenuItem(controlSettingsMenu, props, _("Long tap action"),
                                                  PROP_TAP_ZONE_ACTION_LONG);
        controlSettingsMenu->addItem(touchMenu);

        createSettingsMenuItem(controlSettingsMenu, mm_TapZoneSize, _("Height of the upper tap zone"),
                               valueFont, PROP_TAP_ZONE_HEIGHT_1, tap_zone_sizes);
        createSettingsMenuItem(controlSettingsMenu, mm_TapZoneSize, _("Height of the middle tap zone"),
                               valueFont, PROP_TAP_ZONE_HEIGHT_2, tap_zone_sizes);
        createSettingsMenuItem(controlSettingsMenu, mm_TapZoneSize, _("Width of the left tap zone"),
                               valueFont, PROP_TAP_ZONE_WIDTH_1, tap_zone_sizes);
        createSettingsMenuItem(controlSettingsMenu, mm_TapZoneSize, _("Width of the middle tap zone"),
                               valueFont, PROP_TAP_ZONE_WIDTH_2, tap_zone_sizes);

    }
    if ( !controlSettingsMenu->getItems().empty()) {
        controlSettingsMenu->reconfigure( 0 );
        mainMenu->addItem( controlSettingsMenu );
    } else {
        delete controlSettingsMenu;
        controlSettingsMenu = NULL;
    }
    /*
        SetTimeMenuItem * setTime = new SetTimeMenuItem( mainMenu, mm_SetTime, _wm->translateString("VIEWER_MENU_SET_TIME", "Set time"),
                LVImageSourceRef(), menuFont, L"bla" );
        mainMenu->addItem( setTime );
*/

    CRMenu * stylesMenu = new CRMenu(_wm, mainMenu, _menuItemId++,
                                     _("Style settings"),
                                     LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef() );
    stylesMenu->setAccelerators( _menuAccelerators );
    stylesMenu->setSkinName(lString16("#settings"));

    createSettingsMenuItem(stylesMenu, mm_EmbeddedStyles, _("Document embedded styles"),
                           valueFont, PROP_EMBEDDED_STYLES, on_off_option);
    for ( int i=0; css_styles[i].translate_default; i++) {
        CRMenu * styleMenu = new CRMenu(_wm, stylesMenu, _menuItemId++,
                                        lString16(css_styles[i].translate_default),
                                        LVImageSourceRef(), LVFontRef(), valueFont, CRPropRef() );
        styleMenu->setAccelerators( _menuAccelerators );
        styleMenu->setSkinName(lString16("#settings"));
        createStyleMenuItems(styleMenu, valueFont, css_styles[i].value );
        styleMenu->reconfigure( 0 );
        stylesMenu->addItem( styleMenu );
    }
    stylesMenu->reconfigure( 0 );
    mainMenu->addItem( stylesMenu );

    reconfigure(0);

    if( forcePartialBwUpdates )
        SetWeakTimer("FullUpdate", FullUpdate, 150);
}

/// use to override status text
lString16 CRSettingsMenu::getStatusText()
{
    /// find key by command
    int applyKey = 0;
    int applyFlags = 0;
    int cancelKey = 0;
    int cancelFlags = 0;
    if ( !_acceleratorTable->findCommandKey( MCMD_OK, 0, applyKey, applyFlags )
        || !_acceleratorTable->findCommandKey( MCMD_CANCEL, 0, cancelKey, cancelFlags ) )
        return _statusText;
    lString16 pattern(_("Press $1 to change option\n$2 to apply, $3 to cancel"));
#ifdef CR_POCKETBOOK
	pattern.replaceParam(1, getCommandKeyName(MCMD_SELECT) );
#else
    pattern.replaceParam(1, getItemNumberKeysName());
#endif
    pattern.replaceParam(2, getCommandKeyName(MCMD_OK) );
    pattern.replaceParam(3, getCommandKeyName(MCMD_CANCEL) );
    return pattern;
}
