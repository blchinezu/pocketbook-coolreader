//
// C++ Interface: pb_toc
//

#ifndef CR3_PB_TOC_H_INCLUDED
#define CR3_PB_TOC_H_INCLUDED

#include <inkview.h>
#include "mainwnd.h"
#include "settings.h"

#define PB_TOC_SAFE_CMD_RANGE 90000

class V3DocViewWin;

extern lString16 pbSkinFileName;
extern V3DocViewWin * main_win;

class CRTocMenu : public CRFullScreenMenu
{
    protected:
        CRPropRef props;
        CRGUIAcceleratorTableRef _menuAccelerators;
    public:
        CRTocMenu( CRGUIWindowManager * wm, CRPropRef props, int id, CRGUIAcceleratorTableRef menuAccelerators, lvRect & rc, tocentry *tocItems, int length );
        virtual bool onCommand( int command, int params );
        virtual ~CRTocMenu()
        {
            forcePartialBwUpdates = false;
            CRLog::trace("Calling fontMan->gc() on Toc menu destroy");
            fontMan->gc();
            CRLog::trace("Done fontMan->gc() on Toc menu destroy");
        }
    private:
        int _menuItemId;
};

#endif //CR3_PB_TOC_H_INCLUDED
