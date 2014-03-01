//
// C++ Implementation: links navigation dialog
//
// Description:
//      Allows to select link from current page, and go to it.
//
// Author: Vadim Lopatin <vadim.lopatin@coolreader.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "linksdlg.h"
#include "mainwnd.h"

void CRLinksDialog::Update()
{
    setDirty();
    _wm->update( false );
}

void CRLinksDialog::draw()
{
    if ( _invalidateRect.isEmpty() ) {
        CRLog::debug("crLinksDialog::Draw() - invalidated rect is empty!");
        return;
    }
    CRLog::debug("crLinksDialog::Draw()");
    LVDocImageRef page = _docview->getPageImage(0);
    LVDrawBuf * buf = page->getDrawBuf();
    _wm->getScreen()->draw( buf );
    if (!_controlsCreated) {
        ldomXRangeList list;
        _docview->getCurrentPageLinks( list );
        CRLog::trace("%d links found on the page", list.length());
        for (int i=0; i<list.length(); i++) {
            ldomXRange * link = list[i];
            LVArray<lvRect> rects;
            if ( link && link->getRectangles( rects ) ) {
                for ( int j=0; j<rects.length(); j++ ) {
                    lvRect rc = rects[j];
                    lvPoint topLeft = rc.topLeft();
                    lvPoint bottomRight = rc.bottomRight();
                    if ( _docview->docToWindowPoint(topLeft) && _docview->docToWindowPoint(bottomRight) ) {
                        rc.left = topLeft.x;
                        rc.top = topLeft.y;
                        rc.right = bottomRight.x;
                        rc.bottom = bottomRight.y;
                        CRLog::trace("Link control at[%d,%d,%d,%d]", rc.left, rc.top, rc.right, rc.bottom);
                        addControl( new CRCommandControl(this, rc, MCMD_SELECT, i) );
                    } else {
                        CRLog::error("link rect conversion error");
                    }
                }
            } else {
                CRLog::trace("Couldn't get the rectangle of the link range");
            }
        }
        addControl(new CRClientControl(this, _invalidateRect));
        _controlsCreated = true;
    }
    if ( NULL!=_toolBar)
        _toolBar->draw();
}

CRLinksDialog * CRLinksDialog::create( CRGUIWindowManager * wm, CRViewDialog * docwin, bool backPreffered )
{
    ldomXRangeList list;
    docwin->getDocView()->getCurrentPageLinks( list );
    int backSize = docwin->getDocView()->getNavigationHistory().backCount();
    int fwdSize = docwin->getDocView()->getNavigationHistory().forwardCount();
    if ( list.length()==0 && backSize==0 && fwdSize==0)
        return NULL;
    docwin->getDocView()->clearImageCache();
    docwin->getDocView()->selectFirstPageLink();
    return new CRLinksDialog( wm, docwin, backPreffered);
}

class LinksToolbar : public CRToolBar
{
public:
    LinksToolbar(CRGUIWindowBase *window, lString16 id, CRToolBarSkinRef tbskin, bool active = true)
        :  CRToolBar(window, id, tbskin, active)
    {
        init();
    }
    ~LinksToolbar()
    {}
    void initDefault()
    {
        addButton( new CRToolButton(this, 0, cs16("<-"), DCMD_LINK_BACK, 0) ); //back
        addButton( new CRToolButton(this, 1, cs16("->"), DCMD_LINK_FORWARD, 0) ); //forward
        addButton( new CRToolButton(this, 2, cs16(""), MCMD_SWAP, 0) ); //swap
        addButton( new CRToolButton(this, 0, cs16("X"), MCMD_CANCEL, 0) ); // cancel
    }
};

CRLinksDialog::CRLinksDialog( CRGUIWindowManager * wm, CRViewDialog * docwin, bool backPreffered )
: CRGUIWindowBase( wm ), _docwin(docwin), _docview(docwin->getDocView()), _toolBar(NULL)
{
    setSkinName( L"#links-dialog");
    _invalidateRect.left = 0;
    _invalidateRect.top = 0;
    _invalidateRect.right = _wm->getScreen()->getWidth();
    _invalidateRect.bottom = _wm->getScreen()->getHeight();
    CRToolBarSkinRef tb1Skin;
    CRToolBarSkinRef tb2Skin;
    CRWindowSkinRef windowSkin = getSkin();
    if ( !windowSkin.isNull() ) {
        tb1Skin = windowSkin->getToolBar1Skin();
        tb2Skin = windowSkin->getToolBar2Skin();
    }
    if ( !tb1Skin.isNull() ) {
        _toolBar = new LinksToolbar(this, cs16("links-toolbar"), tb1Skin, backPreffered);

        _forwardIndex = _toolBar->findButton(DCMD_LINK_FORWARD, 0);
        if ( -1 == _forwardIndex )
            _forwardIndex = _toolBar->findButton(MCMD_LONG_FORWARD, 0);
        _backIndex = _toolBar->findButton(DCMD_LINK_BACK, 0);
        if ( -1 == _backIndex )
            _backIndex = _toolBar->findButton(MCMD_LONG_BACK, 0);
        if ( tb2Skin.isNull() ) {
            // can't switch toolbar position
            _toolBar->setEnabled(_toolBar->findButton(MCMD_SWAP, 0), false);
        }
    }
    _curPage = -1;
    _fullscreen = true;
    _onTop= true;
    activate(backPreffered);
}

/// returns true if command is processed
bool CRLinksDialog::onCommand( int command, int params )
{
    CRLog::trace("CRLinksDialog::onCommand( %d, %d )", command, params);
    bool needUpdate = false;
    switch ( command ) {
    case MCMD_CANCEL:
        _docview->clearSelection();
        _wm->closeWindow( this );
        return true;
    case MCMD_OK:
        if ( NULL != _toolBar && _toolBar->isActive() ) {
            CRToolButton * button = _toolBar->getCurrentButton();
            if (NULL != button && button->isEnabled())
                _wm->postCommand( button->getCommand(), button->getParam());
            return false;
        } else if ( _docview->goSelectedLink() ) {
            activate( true );
        }
        return true;
    case DCMD_LINK_BACK:
        if (_docview->goBack())
            activate(_docview->canGoBack());
        break;
    case MCMD_LONG_BACK:
        _docview->clearSelection();
        _docview->goBack();
        _wm->closeWindow( this );
        return true;
    case DCMD_LINK_FORWARD:
        if (_docview->goForward())
            activate(false);
        break;
    case MCMD_LONG_FORWARD:
        _docview->clearSelection();
        _docview->goForward();
        _wm->closeWindow( this );
        return true;
    case DCMD_LINK_NEXT:
    case MCMD_SCROLL_FORWARD:
    case MCMD_SELECT_0:
        if ( NULL != _toolBar && _toolBar->isActive() ) {
            if ( !( needUpdate=_toolBar->selectNextButton( _linkCount==0 ))) {
                if ( _docview->selectNextPageLink( true ) ) {
                    needUpdate = true;
                    _toolBar->setActive(false);
                }
            }
        } else if ( !( needUpdate=_docview->selectNextPageLink( false )) ) {
            needUpdate = _toolBar->selectFirstButton();
        }
        break;
    case DCMD_LINK_PREV:
    case MCMD_SCROLL_BACK:
    case MCMD_SELECT_9:
        if ( NULL != _toolBar && _toolBar->isActive() ) {
            if ( !( needUpdate=_toolBar->selectPrevButton( _linkCount==0 ) ) ) {
                if ( _docview->selectPrevPageLink( true ) ) {
                    needUpdate = true;
                    _toolBar->setActive(false);
                }
            }
        } else if ( !( needUpdate=_docview->selectPrevPageLink( false ) )) {
            needUpdate = _toolBar->selectLastButton();
        }
        break;
    case DCMD_LINK_GO:
    case MCMD_SELECT:
        CRLog::trace("selectLink(%d)", params);
        return selectLink(params);
    case MCMD_SELECT_1:
    case MCMD_SELECT_2:
    case MCMD_SELECT_3:
    case MCMD_SELECT_4:
    case MCMD_SELECT_5:
    case MCMD_SELECT_6:
    case MCMD_SELECT_7:
    case MCMD_SELECT_8:
        return selectLink(command - MCMD_SELECT_1);
    case DCMD_LINK_FIRST:
        return selectLink(0);
    case DCMD_LINK_LAST:
        return selectLink( _linkCount-1 );
    case MCMD_SWAP:
        {
            //swap toolbar position
            CRWindowSkinRef windowSkin = getSkin();
            _onTop = !_onTop;
            if (_onTop)
                _toolBar->setSkin(windowSkin->getToolBar1Skin());
            else
                _toolBar->setSkin(windowSkin->getToolBar2Skin());

        }
        break;
    default:
        return true;
    }
    if (needUpdate) {
        Update();
        invalidateCurrentSelection();
    }
    return true;
}

void CRLinksDialog::invalidateCurrentSelection()
{
    if ( NULL != _toolBar && _toolBar->isActive() ) {
        _toolBar->getRect( _invalidateRect );
        CRLog::debug("invalidateCurrentSelection() : invalidating buttons rect");
        return;
    }
    ldomXRange * link = _docview->getCurrentPageSelectedLink();
    _invalidateRect.clear();
    if ( !link ) {
        CRLog::debug("invalidateCurrentSelection() : no current page selection found!");
        return;
    }
    LVArray<lvRect> rects;
    if ( link->getRectangles( rects ) ) {
#if 1
        _invalidateRect.left = 0;
        _invalidateRect.top = 0;
        _invalidateRect.right = _wm->getScreen()->getWidth();
        _invalidateRect.bottom = _wm->getScreen()->getHeight();
#else
        lvPoint topLeft = rc.topLeft();
        lvPoint bottomRight = rc.bottomRight();
        if ( _docview->docToWindowPoint(topLeft) && _docview->docToWindowPoint(bottomRight) ) {
            rc.left = topLeft.x;
            rc.top = topLeft.y;
            rc.right = bottomRight.x;
            rc.bottom = bottomRight.y;
            CRLog::debug("invalidating link screenRect { %d, %d, %d, %d }", rc.left, rc.top, rc.right, rc.bottom);
            _invalidateRect = rc;
        } else {
            CRLog::debug("link rect conversion error: invalidating full screen");
            _invalidateRect.left = 0;
            _invalidateRect.top = 0;
            _invalidateRect.right = 600;
            _invalidateRect.bottom = 800;
        }
#endif
    } else {
        CRLog::debug("invalidateCurrentSelection() : getRect failed for link!");
    }
}

bool CRLinksDialog::selectLink(int index)
{
    if ( index < _linkCount ) {
        ldomXRangeList list;
        _docview->getCurrentPageLinks( list );
        ldomXRange * link = list[index];
        //invalidateCurrentSelection();
        _docview->selectRange( *link );
        //draw();
        invalidateCurrentSelection();
        draw();
        if (_docview->goSelectedLink())
            activate(true);
    }
    return true;
}

bool CRLinksDialog::onClientTouch(lvPoint &pt, CRGUITouchEventType evType)
{
    int tapZone = 0;
    V3DocViewWin *docwin = static_cast<V3DocViewWin *>(_docwin);
    if (docwin)
        tapZone = getTapZone(pt.x, pt.y, docwin->getProps());
    else
        CRLog::error("Invalid window");
    int command = 0;
    if (evType == CRTOUCH_DOWN_LONG) {
        switch(tapZone) {
        case 4:
            command = MCMD_LONG_BACK;
            break;
        case 5:
            command = MCMD_CANCEL;
            break;
        case 6:
            command = MCMD_LONG_FORWARD;
            break;
        default:
            break;
        }
    }
    if ( command != 0 ) {
        _wm->postCommand( command, 0 );
        return true;
    }
    return false;
}

bool CRLinksDialog::activate(bool backPreffered)
{
    bool needUpdate = false;
    int curPage = _docview->getCurPage();
    if (_curPage != curPage) {
        needUpdate = true;
        _curPage = curPage;
        ldomXRangeList list;
        _docview->getCurrentPageLinks( list );
        _linkCount = list.length();
        _backSize = _docview->getNavigationHistory().backCount();
        _fwdSize = _docview->getNavigationHistory().forwardCount();
        if ( NULL != _toolBar ) {
            _toolBar->setEnabled( _backIndex, ( _backSize!=0 ) );
            _toolBar->setEnabled( _forwardIndex, ( _fwdSize!=0 ) );
        }
        if ( (_linkCount > 0 && !backPreffered) || NULL == _toolBar ) {
            if (NULL != _toolBar)
                _toolBar->setActive(false);
            _docview->selectFirstPageLink();
        } else
            _toolBar->selectFirstButton();
        invalidateControls();
    }
    if (needUpdate) {
        Update();
        invalidateCurrentSelection();
    }
    return true;
}
