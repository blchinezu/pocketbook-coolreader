//
// C++ Implementation: pb_toc
//

#include <crgui.h>
#include <cri18n.h>
#include <math.h>
#include "viewdlg.h"
#include "mainwnd.h"
#include "cr3pocketbook.h"
//#include "fsmenu.h"
#include "pb_toc.h"

bool CRTocMenu::onCommand( int command, int params )
{
    CRLog::trace("CRTocMenu::onCommand(%d, %d)", command, params);
    return CRMenu::onCommand( command, params );
}

CRTocMenu::CRTocMenu( CRGUIWindowManager * wm, CRPropRef newProps, int id,
    CRGUIAcceleratorTableRef menuAccelerators, lvRect &rc, tocentry *tocItems,
    int length, int currentPage )
: CRFullScreenMenu( wm, id, lString16(_("Contents")), 8, rc ),
  props( newProps ),
  _menuAccelerators( menuAccelerators ), _menuItemId(mm_TouchTOC)
{
    CRLog::trace("showTocTouchMenu(): Construct");

    if( !fontAntiAliasingActivated() && (
            pbSkinFileName == lString16("pb626fw5.cr3skin") ||
            pbSkinFileName == lString16("pb631fw5.cr3skin")
            )
        ) {
        forcePartialBwUpdates = true;
    }

    setSkinName(lString16("#settings"));

    _fullscreen = true;

    LVFontRef valueFont( fontMan->GetFont(
        VALUE_FONT_SIZE,
        400,
        true,
        css_ff_sans_serif,
        lString8("Arial")
        ) );

    CRMenu * mainMenu = this;
    mainMenu->setAccelerators( _menuAccelerators );

    _selectedItem = 0;

    if ( length ) {

        // CRMenu * subMenus = (CRMenu *) malloc(length * sizeof(CRMenu));
        // int subMenusLength = 0;
        // CRMenu * subMenus[MAX_TOC_SUBMENUS];

        // subMenus[0] = mainMenu;


        for (int i = 0; i < length; i++) {
            if( currentPage >= tocItems[i].position ) {
                _selectedItem = i;
            }
        }

        for (int i = 0; i < length; i++) {

            // _toc[j].level = item->getLevel();
            // _toc[j].position = item->getPage() + 1;
            // _toc[j].page = _toc[j].position;
            // _toc[j].text = strdup(UnicodeToUtf8(item->getName()).c_str());

            // bool gotPrev = i > 0;
            // bool gotNext = i < length-1;

            // int prevLevel = tocItems[i-1 > 0 ? i-1 : 0].level;
            // int currLevel = tocItems[i].level;
            // int nextLevel = tocItems[i+1 < length-1 ? i+1 : length-1].level;

            lString16 text(tocItems[i].text);

            // Prepend page number
            text = L"[" + lString16::itoa(tocItems[i].position) + L"] " + text;

            // Mark current chapter
            if( _selectedItem == i ) {
                text = L" > " + text;
            }

            // Prepend level
            // text = L"[L" + lString16::itoa(tocItems[i].level) + L"] " + text;

            // // If there are submenus to add
            // if( gotPrev && currLevel < prevLevel ) {
            //     for( int level = prevLevel; level >= currLevel; level-- ) {
            //         subMenus[ level-2 ]->addItem( subMenus[ level-1 ] );
            //     }
            // }

            // // Create submenu
            // if( gotNext && nextLevel > currLevel && currLevel < MAX_TOC_SUBMENUS ) {

            //     // Mark submenu by prepending "[+]"
            //     text = L"[+] " + text;

            //     // Create new menu
            //     subMenus[currLevel] = new CRMenu( _wm, subMenus[currLevel-1],
            //         PB_TOC_SAFE_CMD_SUBMENU_RANGE + i, _("Font settings"), "cr3_option_font_settings",
            //         LVFontRef(), valueFont, CRPropRef() );
            //     subMenus[currLevel]->setSkinName(lString16("#settings"));
            //     subMenus[currLevel]->setAccelerators( _menuAccelerators );
            // }

            // // Add item
            // else {

                // Add item
                // subMenus[currLevel-1]->addItem( new CRMenuItem(

                mainMenu->addItem(new CRMenuItem(
                    mainMenu,
                    PB_TOC_SAFE_CMD_RANGE + tocItems[i].page,
                    UnicodeToUtf8(text).c_str(),
                    LVImageSourceRef(),
                    LVFontRef()
                    ));

            // }

        }

        // // If there are submenus to add
        // if( tocItems[length-1].level > 1 ) {
        //     for( int level = tocItems[length-1].level; level > 1; level-- ) {
        //         subMenus[ level-2 ]->addItem( subMenus[ level-1 ] );
        //     }
        // }

    }

    // Empty contents
    else {
        mainMenu->addItem( new CRMenuItem(
            mainMenu,
            PB_TOC_SAFE_CMD_RANGE + 1,
            "Empty TOC",
            LVImageSourceRef(),
            LVFontRef()
            ) );
    }

    reconfigure(0);

    setCurPage( (int)floor(_selectedItem/_pageItems) );

    if( forcePartialBwUpdates ) {
        SetWeakTimer("FullUpdate", FullUpdate, 150);
    }

}
