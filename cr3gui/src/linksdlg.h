//
// C++ Interface: links navigation dialog
//
// Description:
//
//
// Author: Vadim Lopatin <vadim.lopatin@coolreader.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
// linksdlg.h

#ifndef LINKSDLG_H_INCLUDED
#define LINKSDLG_H_INCLUDED

#include "viewdlg.h"


class CRLinksDialog : public CRGUIWindowBase
{
    protected:
        int _cursorPos;
        CRViewDialog * _docwin;
        LVDocView * _docview;
        lvRect _invalidateRect;
        int _linkCount;
        int _backSize;
        int _fwdSize;
        bool activate(bool backPreffered);
        int _curPage;
        CRToolBar *_toolBar;
        bool _onTop;
    protected:
        virtual void Update();
        virtual void draw();
        virtual bool selectLink(int index);
    public:
        static CRLinksDialog * create( CRGUIWindowManager * wm, CRViewDialog * docwin, bool backPreffered=false );
        CRLinksDialog( CRGUIWindowManager * wm, CRViewDialog * docwin, bool backPreffered=false );
        virtual ~CRLinksDialog() { }
        /// returns true if command is processed
        virtual bool onCommand( int command, int params );
        void invalidateCurrentSelection();
        virtual bool onClientTouch(lvPoint &pt, CRGUITouchEventType evType);
};

#endif
