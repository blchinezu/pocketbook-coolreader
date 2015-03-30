//
// C++ Implementation: pb_toc
//

#include "pb_toc.h"
#include <crgui.h>
#include "viewdlg.h"
#include "mainwnd.h"
//#include "fsmenu.h"
#ifdef CR_POCKETBOOK
#include "cr3pocketbook.h"
#endif

#include <cri18n.h>

bool CRTocMenu::onCommand( int command, int params )
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

void CRTocMenu::addMenuItems( CRMenu * menu, item_def_t values[] )
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

CRMenu * CRTocMenu::createTocMenuItem(CRMenu * menu, int id, const char * label, LVFontRef valueFont, const char * propName, item_def_t values[])
{
    CRMenu * menuItem;
    menuItem = new CRMenu(_wm, menu, id, label,
                          LVImageSourceRef(), LVFontRef(), valueFont, props, propName);
    addMenuItems(menuItem, values);
    menu->addItem( menuItem );
    return menuItem;
}

CRTocMenu::CRTocMenu( CRGUIWindowManager * wm, CRPropRef newProps, int id, CRGUIAcceleratorTableRef menuAccelerators, lvRect &rc, tocentry *_toc )
: CRFullScreenMenu( wm, id, lString16(_("Contents")), 8, rc ),
  props( newProps ),
  _menuAccelerators( menuAccelerators ), _menuItemId(mm_Last)
{
    setSkinName(lString16("#toc"));

    _fullscreen = true;

    CRLog::trace("showTocMenu() - %d property values found", props->getCount() );

    setSkinName(lString16("#toc"));

    // ==========================================================================
    
    if (_toc != NULL)
        freeContents();
    LVPtrVector<LVTocItem, false> tocItems;
    _docview->getFlatToc(tocItems);
    _tocLength = tocItems.length();

    if (_tocLength) {
        int tocSize = (_tocLength + 1) * sizeof(tocentry);
        _toc = (tocentry *) malloc(tocSize);
        int j = 0;
        for (int i = 0; i < tocItems.length(); i++) {
            LVTocItem * item = tocItems[i];
            if (item->getName().empty())
                continue;
            _toc[j].level = item->getLevel();
            _toc[j].position = item->getPage() + 1;
            _toc[j].page = _toc[j].position;
            _toc[j].text = strdup(UnicodeToUtf8(item->getName()).c_str());
            char *p = _toc[j++].text;
            while (*p) {
                if (*p == '\r' || *p == '\n') *p = ' ';
                p++;
            }
        }
        _tocLength = j;
        if (j == 0) {
            free(_toc);
            _toc = NULL;
        }
    }
    if (!_tocLength) {
        Message(ICON_INFORMATION, const_cast<char*>("CoolReader"),
                const_cast<char*>("@No_contents"), 2000);
        return;
    }
    CRPocketBookContentsWindow *wnd = new CRPocketBookContentsWindow(_wm, _toc,
                                                                     _tocLength, _docview->getCurPage() + 1);
    _wm->activateWindow( wnd );

    // ==========================================================================

    LVFontRef valueFont( fontMan->GetFont( VALUE_FONT_SIZE, 400, true, css_ff_sans_serif, lString8("Arial")) );
    CRMenu * mainMenu = this;
    mainMenu->setAccelerators( _menuAccelerators );

    createTocMenuItem(mainMenu, mm_Embolden, _("Font weight"),
                           valueFont, PROP_FONT_WEIGHT_EMBOLDEN, embolden_mode);
    createTocMenuItem(mainMenu, mm_FontAntiAliasing, _("Font antialiasing"),
                           valueFont, PROP_FONT_ANTIALIASING, antialiasing_modes);
    createTocMenuItem(mainMenu, mm_FontHinting, _("Font hinting"),
                           valueFont, PROP_FONT_HINTING, font_hinting);
    createTocMenuItem(mainMenu, mm_Kerning, _("Font kerning"),
                           valueFont, PROP_FONT_KERNING_ENABLED, on_off_option);
    createTocMenuItem(mainMenu, mm_fontGamma, _("Font Gamma"),
                           valueFont, PROP_FONT_GAMMA, font_gammas);
    createTocMenuItem(mainMenu, mm_EmbeddedFonts, _("Document embedded fonts"),
                           valueFont, PROP_EMBEDDED_FONTS, on_off_option);

    reconfigure(0);
}
