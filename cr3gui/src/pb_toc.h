//
// C++ Interface: pb_toc
//

#ifndef CR3_PB_TOC_H_INCLUDED
#define CR3_PB_TOC_H_INCLUDED

#include "settings.h"

class CRTocMenu : public CRFullScreenMenu
{
    protected:
        CRPropRef props;
        CRGUIAcceleratorTableRef _menuAccelerators;
        void addMenuItems( CRMenu * menu, item_def_t values[] );

        lString16 getStatusText();
    public:
        CRMenu * createFontSizeMenu( CRMenu * mainMenu, int *fontSizes, unsigned sizesCount, CRPropRef props, const char * propName );
        #if CR_INTERNAL_PAGE_ORIENTATION==1 || defined(CR_POCKETBOOK)
        CRMenu * createOrientationMenu( CRMenu * mainMenu, CRPropRef props );
        #endif
        CRTocMenu( CRGUIWindowManager * wm, CRPropRef props, int id, LVFontRef font, CRGUIAcceleratorTableRef menuAccelerators, lvRect & rc );
        virtual bool onCommand( int command, int params );
        virtual ~CRTocMenu()
        {
            CRLog::trace("Calling fontMan->gc() on Toc menu destroy");
            fontMan->gc();
            CRLog::trace("Done fontMan->gc() on Toc menu destroy");
        }
    private:
        CRMenu * createTocMenuItem(CRMenu * menu, int id, const char * label, LVFontRef valueFont, const char * propName, item_def_t values[]);
        void createStyleMenuItems( CRMenu * menu, LVFontRef valueFont, const char * prefix );
        int _menuItemId;
};

#endif //CR3_PB_TOC_H_INCLUDED
