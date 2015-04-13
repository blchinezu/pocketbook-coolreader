//
// C++ Implementation: pb_toc
//

#include <crgui.h>
#include <cri18n.h>
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

CRTocMenu::CRTocMenu( CRGUIWindowManager * wm, CRPropRef newProps, int id, CRGUIAcceleratorTableRef menuAccelerators, lvRect &rc, tocentry *tocItems, int length )
: CRFullScreenMenu( wm, id, lString16(_("Contents")), 8, rc ),
  props( newProps ),
  _menuAccelerators( menuAccelerators ), _menuItemId(mm_Last)
{
    CRLog::trace("showTocTouchMenu(): Construct");

    if( pbSkinFileName == lString16("pb626fw5.cr3skin") )
        forcePartialBwUpdates = true;

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

    if ( length ) {
        for (int i = 0; i < length; i++) {

            // _toc[j].level = item->getLevel();
            // _toc[j].position = item->getPage() + 1;
            // _toc[j].page = _toc[j].position;
            // _toc[j].text = strdup(UnicodeToUtf8(item->getName()).c_str());

            char *text = tocItems[i].text;

            // Generate pre text
            // lString16 pre;
            // for( int l = 0; l < tocItems[i].level; l++ )
            //     pre = pre + "- ";

            // Prepend text
            // if( !pre.empty() ) {
            //     free(text);
            //     text = UnicodeToUtf8( pre + lString16(text) ).c_str();
            // }

            // Add item
            mainMenu->addItem( new CRMenuItem(
                mainMenu, 
                PB_TOC_SAFE_CMD_RANGE + tocItems[i].page,
                text,
                LVImageSourceRef(),
                LVFontRef()
                ) );
        }
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
    
    if( forcePartialBwUpdates )
        SetWeakTimer("FullUpdate", FullUpdate, 150);

}
