#include <lvstring.h>
#include <lvref.h>
#include <lvarray.h>
#include <lvtinydom.h>
#include <lvdocview.h>

#include <crgui.h>
#include <crtrace.h>
#include <cri18n.h>

#include "mainwnd.h"
#include "bgfit.h"
#include "citedlg.h"
#include "citecore.h"
#ifdef CR_POCKETBOOK
#include "cr3pocketbook.h"
#endif

class CiteWindow : public BackgroundFitWindow
{
    CiteSelection selector_;
	V3DocViewWin * mainwin_;
#ifdef CR_POCKETBOOK
	int _selectedIndex;
	int _itemsCount;
	lvPoint _lvBtnP;
        bool _bTouchImgs;
        int _shiftX;
        bool _noClose;

	int getCommandFromIndex(int command) 
	{
		int index= _selectedIndex;
		if ( _bTouchImgs )
		   index -= 1;
//		switch (_selectedIndex) {
		switch ( index) {
		case 0:
			//move
			if (command == MCMD_SCROLL_BACK)
				return MCMD_SELECT_5; //up
			else if (command == MCMD_SCROLL_FORWARD)
				return MCMD_SELECT_6; //down
			break;
		case 1:
			//grow
			if (command == MCMD_SCROLL_BACK)
				return MCMD_SELECT_1; //up
			else if (command == MCMD_SCROLL_FORWARD)
				return MCMD_SELECT_0; //down
			break;
		case 2:
			//shrink
			if (command == MCMD_SCROLL_BACK)
				return MCMD_SELECT_9; 
			else if (command == MCMD_SCROLL_FORWARD)
				return MCMD_SELECT_2;
			break;
		case 3:
			// grow phrase:
			if (command == MCMD_SCROLL_BACK)
				return MCMD_SELECT_3; // up
			else if (command == MCMD_SCROLL_FORWARD)
				return MCMD_SELECT_8; // down
		case 4:
			// Shrink phrase
			if (command == MCMD_SCROLL_BACK)
				return MCMD_SELECT_7;
			else if (command == MCMD_SCROLL_FORWARD)
				return MCMD_SELECT_4;
			break;
		}
		return -1;
	}
#endif
protected:
    virtual void draw()
    {
        BackgroundFitWindow::draw();
        CRRectSkinRef skin = _wm->getSkin()->getWindowSkin( L"#dialog" )->getClientSkin();
        LVDrawBuf * buf = _wm->getScreen()->getCanvas().get();
        skin->draw( *buf, _rect );
        lvRect borders = skin->getBorderWidths();
#ifdef CR_POCKETBOOK
        lString16 prompt;
        switch (_selectedIndex) {
        case 0:
            prompt = lString16(_("Select next/prev paragraph"));
            break;
        case 1:
            prompt = lString16(_("Select one more paragraph"));
            break;
        case 2:
            prompt = lString16(_("Deselect paragraph"));
            break;
        case 3:
            prompt = lString16(_("Select one more phrase"));
            break;
        case 4:
            prompt = lString16(_("Deselect phrase"));
            break;

        }
        lvRect keyRect = _rect;
//        CRLog::trace("CiteWindow::draw() keyRect ( %d, %d, %d, %d )", keyRect.left, keyRect.top, keyRect.right, keyRect.bottom );

        int promptWidth = skin->measureText(prompt).x;
        keyRect.right = keyRect.left + promptWidth + borders.left + borders.right;
        if ( !keyRect.isEmpty() ) 
        {
            skin->draw( *_wm->getScreen()->getCanvas(), keyRect );
            if ( !_bTouchImgs )
               skin->drawText( *_wm->getScreen()->getCanvas(), keyRect, prompt );
//            CRLog::trace("CiteWindow::draw() drawText keyRect ( %d, %d, %d, %d )", keyRect.left, keyRect.top, keyRect.right, keyRect.bottom );
        }

//        CRToolBarSkinRef tbSkin = _wm->getSkin()->getToolBarSkin( L"#cite-toolbar" );
        CRToolBarSkinRef tbSkin;
        if ( !_bTouchImgs )
           tbSkin = _wm->getSkin()->getToolBarSkin( L"#cite-toolbar" );
       else
           tbSkin = _wm->getSkin()->getToolBarSkin( L"#cite-toolbar-touch" );
           
        if (!tbSkin.isNull()) 
        {
            if ( !_bTouchImgs )
               keyRect.left += (borders.right + _wm->getScreen()->getWidth() * 2/3/*promptWidth*/);
            else            
               keyRect.left += _shiftX;
            
            keyRect.right = _rect.right;

            CRButtonListRef buttons = tbSkin->getButtons();
//            CRLog::trace("CiteWindow::draw() tbSkin keyRect ( %d, %d, %d, %d ) buttons.isNull()=%d _itemsCount=%d buttons->length()= %d", keyRect.left, keyRect.top, keyRect.right, keyRect.bottom, buttons.isNull(), _itemsCount, buttons->length() );
            if ( !( buttons.isNull() || _itemsCount != buttons->length() ) )
            {
                tbSkin->drawToolBar(*_wm->getScreen()->getCanvas(), keyRect, true, _selectedIndex);
                CRLog::trace("CiteWindow::draw() tbSkin->drawToolBar keyRect ( %d, %d, %d, %d ) _selectedIndex=%d buttons.isNull()=%d _itemsCount=%d buttons->length()= %d", keyRect.left, keyRect.top, keyRect.right, keyRect.bottom, _selectedIndex, buttons.isNull(), _itemsCount, buttons->length() );
            }
        }
#else
    lString16 prompt(_("Select text"));
		buf->FillRect( _rect, 0xAAAAAA );
		lvRect keyRect = _rect;
		LVFontRef font = fontMan->GetFont( 20, 600, false, css_ff_sans_serif, lString8("Arial")); //skin->getFont();
//        int margin = 4;
		keyRect.right = _rect.right;
		if ( !keyRect.isEmpty() ) {
			skin->draw( *_wm->getScreen()->getCanvas(), keyRect );
			skin->drawText( *_wm->getScreen()->getCanvas(), keyRect, prompt );
		}
#endif
    }

public:

	CiteWindow( CRGUIWindowManager * wm, V3DocViewWin * mainwin) :
		BackgroundFitWindow(wm, mainwin),
		selector_(*mainwin->getDocView()),
		mainwin_(mainwin)
    {
		CRGUIAcceleratorTableRef acc = _wm->getAccTables().get("cite");
		if ( acc.isNull() )
			this->setAccelerators( mainwin->getDialogAccelerators() );
		else
			setAccelerators( acc );
		_rect = _wm->getScreen()->getRect();
		//_rect.bottom = _rect.top;
		_rect.top = _rect.bottom - 40;
        selector_.highlight();
        setDirty();
#ifdef CR_POCKETBOOK
      _itemsCount =5;
      _selectedIndex = 0;

      _bTouchImgs= false;
      _noClose= true;
      _shiftX= 0;
      lvPoint pnmax;

      CRRectSkinRef skin = _wm->getSkin()->getWindowSkin( L"#dialog" )->getClientSkin();
      if ( !skin.isNull() )
        _shiftX = skin->getBorderWidths().left;

      CRToolBarSkinRef tbSkin = _wm->getSkin()->getToolBarSkin( L"#cite-toolbar-touch" );
      if ( !tbSkin.isNull() ) 
      {
        _shiftX += tbSkin->getBorderWidths().left;

        CRButtonListRef buttons = tbSkin->getButtons();
        LVRef<CRButtonSkin> button;
        LVImageSourceRef barImg;
        if ( !buttons.isNull() ) 
          button = buttons->get( 0 );

        if ( !button.isNull() ) 
          barImg= button->getNormalImage();

        if ( !barImg.isNull() ) 
          _lvBtnP= lvPoint( barImg->GetWidth(), barImg->GetHeight() );
        
        CRLog::trace("CiteWindow  barImg= (%d, %d) _shiftX= %d", _lvBtnP.x, _lvBtnP.y, _shiftX );
//        _lvBtnP= tbSkin->getMinSize();// Load image 
//        pnmax= tbSkin->getMaxSize();
//        CRLog::trace("CiteWindow  MinSize()  %d, %d MaxSize() %d, %d  _shiftX=%d ", _lvBtnP.x, _lvBtnP.y, pnmax.x, pnmax.y, _shiftX );
//      if ( _lvBtnP.y < pnmax.y )
//        _lvBtnP.y=  pnmax.y;
        
        if ( _lvBtnP.y > 40 )
        {
          _lvBtnP.y += 16;//for border
          _rect.top = _rect.bottom - _lvBtnP.y;
        }

        if ( _lvBtnP.y && _lvBtnP.x && buttons->length() == 7 )
        {
          _itemsCount = buttons->length();
          _selectedIndex = 1;
          _bTouchImgs= true;
          CRLog::trace( "CiteWindow  TouchImgs present" );
        }
      }
      
#endif
	}

	bool onCommand( int command, int params )
	{
		switch ( command ) {
			case MCMD_SELECT_1:
                selector_.growUp();
                setDirty();
                break;
			case MCMD_SELECT_2:
                selector_.shrinkDown();
                setDirty();
                break;
			case MCMD_SELECT_5:
                selector_.moveUp();
                setDirty();
                break;
			case MCMD_SELECT_6:
                selector_.moveDown();
                setDirty();
                break;
			case MCMD_SELECT_3:
                selector_.growUpPhrase();
                setDirty();
                break;
			case MCMD_SELECT_4:
                selector_.shrinkDownPhrase();
                setDirty();
                break;
			case MCMD_SELECT_7:
                selector_.shrinkUpPhrase();
                setDirty();
                break;
			case MCMD_SELECT_8:
                selector_.growDownPhrase();
                setDirty();
                break;
			case MCMD_SELECT_9:
                selector_.shrinkUp();
                setDirty();
                break;
			case MCMD_SELECT_0:
                selector_.growDown();
                setDirty();
				break;
			case MCMD_SCROLL_FORWARD:
			case MCMD_SCROLL_BACK:
#ifdef CR_POCKETBOOK
				{
					int cmd = getCommandFromIndex(command);
					if (cmd != -1)
						_wm->postCommand(cmd, params);
				}
#endif			
				break;
			case MCMD_OK:
				{
					ldomXRange range;
					selector_.getRange(range);
					if ( !range.isNull() ) {
                        mainwin_->getDocView()->saveRangeBookmark( range, bmkt_comment, lString16::empty_str);
                        mainwin_->saveHistory(lString16::empty_str);
					}
					close();
				};
				break;
			case MCMD_CANCEL:
                close();
				break;
#ifdef CR_POCKETBOOK
			case PB_CMD_RIGHT:
				if (_selectedIndex == _itemsCount -1)
					_selectedIndex = 0;
				else
					_selectedIndex += 1;
				setDirty();
				break;
			case PB_CMD_LEFT:
				if (_selectedIndex == 0)
					_selectedIndex = _itemsCount -1;
				else
					_selectedIndex -= 1;
				setDirty();
				break;
#endif
		}
		return true;
	}

#ifdef CR_POCKETBOOK
    virtual void covered() 
    {
       if ( _noClose )
       {
//         CRLog::trace( "CiteWindow::covered(). Lost focus, set activate this window.");
         CRGUIWindowManager * wm = 0;
         if ( this && ( wm = mainwin_->getWindowManager() ) )
           wm->activateWindow( this );
         else
           close();
       }
    }

    virtual bool onTouchEvent( int x, int y, CRGUITouchEventType evType );
    int getTapItem( int x, int y );

#endif


    void close() {
        _noClose= false;
        CRLog::info("Closing cite");
        _mainwin->getDocView()->clearSelection();
        _mainwin->getDocView()->updateBookMarksRanges();
        _wm->closeWindow(this);
    }

protected:
    CiteWindow(const CiteWindow&); //non-copyable
};

#ifdef CR_POCKETBOOK

int CiteWindow::getTapItem( int x, int y )
{
  int item= -1;
  lvPoint pn( x, y );
  lvRect keyRect = _rect;

  keyRect.left += _shiftX;
  if ( _rect.right < keyRect.left + _lvBtnP.x * _itemsCount )
    keyRect.right += keyRect.left + _lvBtnP.x * _itemsCount;

  if ( !keyRect.isPointInside( pn ) || !_bTouchImgs )
    return item;

  if ( _itemsCount )
  {
    int xp= x - _shiftX;
    if ( xp >= 0 )
      item= xp/_lvBtnP.x;

//    CRLog::trace("CiteWindow::getTapItem() keyRect ( %d, %d, %d, %d ) xp=%d keyRect.width()=%d _lvBtnP.x=%d item=%d", keyRect.left, keyRect.top, keyRect.right, keyRect.bottom, xp, keyRect.width(), _lvBtnP.x, item );

    if ( item >= _itemsCount )
      item= -1;
  }

  CRLog::trace("CiteWindow::getTapItem() item= %d", item );

  return item;
}

bool CiteWindow::onTouchEvent( int x, int y, CRGUITouchEventType evType )
{
      CRLog::trace("CiteWindow::onTouchEvent() x=%d  y= %d evType= %d", x, y, int( evType ) );

      int item= -1;

      switch ( evType )
      {
      //case CRTOUCH_DOWN:
      //CRLog::trace("CiteWindow::onTouchEvent() DOWN %d",  evType );
      //break;
      //
      //case CRTOUCH_DOWN_LONG:
      //CRLog::trace("CiteWindow::onTouchEvent() DOWN_LONG %d",  evType );
      //break;
      
      case CRTOUCH_UP:
        {
          CRLog::trace("CiteWindow::onTouchEvent() x=%d  y= %d evType= %d", x, y, int( evType ) );

          item= getTapItem( x, y );
          if ( item < 0 ) 
            return true;

          switch ( item )
          {
          case 0:
            {
              ldomXRange range;
              selector_.getRange(range);
              if ( !range.isNull() ) {
                mainwin_->getDocView()->saveRangeBookmark( range, bmkt_comment, lString16::empty_str);
                mainwin_->saveHistory(lString16::empty_str);
              }
              close();
            };
            break;
          case 6:
            close();
            break;

          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
              _selectedIndex = item;
              setDirty();
              CRLog::trace("CiteWindow::onTouchEvent() _selectedIndex %d",  _selectedIndex );
            break;

          default:
            break;
          }//switch item
        }
        break;

      default:
        break;

      }

      return true;
}

#endif

void activate_cite( CRGUIWindowManager *wm, V3DocViewWin * mainwin)
{
    CRLog::info("Entering cite mode\n");
    wm->activateWindow(new CiteWindow(wm, mainwin));
}
