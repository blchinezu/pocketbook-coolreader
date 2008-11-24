/***************************************************************************
 *   Copyright (C) 2007 by Vadim Lopatin   *
 *   vadim.lopatin@coolreader.org   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "lvtypes.h"
#include "lvptrvec.h"
#include "lvdrawbuf.h"

#ifdef CR_WX_SUPPORT
#include <wx/wx.h>
#endif

class CRGUIWindowManager;

/// Screen object - provides canvas and interface to device screen
class CRGUIScreen
{
    public:
        /// creates compatible canvas of specified size
        virtual LVDrawBuf * createCanvas( int dx, int dy ) = 0;
        /// sets new screen size, returns true if size is changed
        virtual bool setSize( int dx, int dy ) { return false; }
        /// returns screen width
        virtual int getWidth() = 0;
        /// returns screen height
        virtual int getHeight() = 0;
        /// returns screen dimension
        virtual lvRect getRect() { return lvRect(0, 0, getWidth(), getHeight() ); }
        /// return pointer to screen canvas
        virtual LVRef<LVDrawBuf> getCanvas() = 0;
        /// draw image on screen canvas
        virtual void draw( LVDrawBuf * img, int x = 0, int y = 0) = 0;
        /// transfers contents of buffer to device, if full==true, redraws whole screen, otherwise only changed area
        virtual void update( bool full ) { }
        /// invalidates rectangle: add it to bounding box of next partial update
        virtual void invalidateRect( const lvRect & rc ) { }
        virtual ~CRGUIScreen() { }
};

/// Window interface
class CRGUIWindow
{
    public:
        /// returns true if key is processed
        virtual bool onKeyPressed( int key, int flags = 0 ) { return false; }
        /// returns true if command is processed
        virtual bool onCommand( int command, int params = 0 ) { return false; }
        /// returns true if window is visible
        virtual bool isVisible() const = 0;
        /// returns true if window is fullscreen
        virtual bool isFullscreen() = 0;
        /// returns true if window is changed but now drawn
        virtual bool isDirty() = 0;
        /// shows or hides window
        virtual void setVisible( bool visible ) = 0;
        /// returns window rectangle
        virtual const lvRect & getRect() const = 0;
        /// sets window rectangle
        virtual void setRect( const lvRect & rc ) = 0;
        /// draws content of window to screen
        virtual void flush() = 0;
        /// called if window gets focus
        virtual void activated() { }
        /// called if window loss focus
        virtual void covered() { }
        /// returns window manager
        virtual CRGUIWindowManager * getWindowManager() = 0;
        /// destroys window
        virtual ~CRGUIWindow() { }
};

/// Window manager
class CRGUIWindowManager
{
    protected:
        LVPtrVector<CRGUIWindow, true> _windows;
        CRGUIScreen * _screen;
    public:
        /// sets new screen size
        virtual void setSize( int dx, int dy )
        {
            if ( _screen->setSize( dx, dy ) ) {
                lvRect fullRect = _screen->getRect();
                for ( int i=_windows.length()-1; i>=0; i-- ) {
                    lvRect rc = _windows[i]->getRect();
                    if ( _windows[i]->isFullscreen() )
                        _windows[i]->setRect( fullRect );
                    else {
                        if ( rc.right > dx ) {
                            rc.left -= rc.right - dx;
                            rc.right = dx;
                            if ( rc.left < 0 )
                                rc.left = 0;
                        }
                        if ( rc.right > dx ) {
                            rc.left -= rc.right - dx;
                            rc.right = dx;
                            if ( rc.left < 0 )
                                rc.left = 0;
                        }
                        _windows[i]->setRect( rc );
                    }
                }
                update( true );
            }
        }
        /// returns true if command is processed
        virtual bool onCommand( int command, int params = 0 )
        {
            for ( int i=_windows.length()-1; i>=0; i-- ) {
                if ( _windows[i]->onCommand( command, params ) )
                    return true;
            }
            return false;
        }
        /// returns true if key is processed
        virtual bool onKeyPressed( int key, int flags = 0 )
        {
            for ( int i=_windows.length()-1; i>=0; i-- ) {
                if ( _windows[i]->onKeyPressed( key, flags ) )
                    return true;
            }
            return false;
        }
        /// activates window, brings it on top; add to stack if not added
        void activateWindow( CRGUIWindow * window )
        {
            int index = _windows.indexOf( window );
            CRGUIWindow * lostFocus = _windows.peek();
            if ( index < 0 ) {
                _windows.push( window );
            } else if ( index < _windows.length() - 1 ) {
                _windows.push( _windows.remove( index ) );
            }
            if ( window != lostFocus )
            {
                if ( lostFocus )
                    lostFocus->covered();
                window->activated();
            }
        }
        /// closes window, removes from stack, destroys object
        void closeWindow( CRGUIWindow * window )
        {
            int index = _windows.indexOf( window );
            if ( index >= 0 ) {
                if ( window == _windows.peek() )
                    window->covered(); // send cover before close
                _windows.remove( index );
            }
            delete window;
        }
        /// redraw one window
        void updateWindow( CRGUIWindow * window )
        {
            int index = _windows.indexOf( window );
            if ( index < 0 )
                return;
            lvRect coverBox;
            if  ( _windows.empty() )
                return;
            LVPtrVector<CRGUIWindow, false> drawList;
            for ( int i=_windows.length()-1; i>=index; i-- ) {
                if ( !_windows[i]->isVisible() )
                    continue;
                lvRect rc = _windows[i]->getRect();
                if ( coverBox.isRectInside( rc ) )
                    continue; // fully covered by top window
                if ( !rc.isEmpty() )
                    drawList.add( _windows[i] );
                if ( !rc.isRectInside( coverBox ) )
                    coverBox = rc;
            }
            while ( !drawList.empty()  ) {
                CRGUIWindow * w = drawList.pop();
                if ( w->isDirty() ) {
                    w->flush();
                    _screen->invalidateRect( w->getRect() );
                }
            }
        /// invalidates rectangle: add it to bounding box of next partial update
            _screen->update( false );
        }
        /// full redraw of all windows
        void update( bool fullScreenUpdate )
        {
            lvRect coverBox;
            if  ( _windows.empty() )
                return;
            LVPtrVector<CRGUIWindow, false> drawList;
            for ( int i=_windows.length()-1; i>=0; i-- ) {
                if ( !_windows[i]->isVisible() )
                    continue;
                lvRect rc = _windows[i]->getRect();
                if ( coverBox.isRectInside( rc ) )
                    continue; // fully covered by top window
                if ( !rc.isEmpty() )
                    drawList.add( _windows[i] );
                if ( !rc.isRectInside( coverBox ) )
                    coverBox = rc;
            }
            while ( !drawList.empty()  ) {
                CRGUIWindow * w = drawList.pop();
                if ( w->isDirty() || fullScreenUpdate ) {
                    w->flush();
                    _screen->invalidateRect( w->getRect() );
                }
            }
            _screen->update( fullScreenUpdate );
        }
        /// returns screen associated with window manager
        CRGUIScreen * getScreen()
        {
            return _screen;
        }
        CRGUIWindowManager(CRGUIScreen * screen)
        : _screen( screen )
        {
        }
        virtual void closeAllWindows()
        {
            for ( int i=_windows.length()-1; i>=0; i-- ) {
                closeWindow(_windows[i]);
            }
        }
        /// destroy all windows on close
        virtual ~CRGUIWindowManager()
        {
            closeAllWindows();
        }
};

/// Window base implementation
class CRGUIWindowBase : public CRGUIWindow
{
    protected:
        CRGUIWindowManager * _wm;
        lvRect _rect;
        bool _visible;
        bool _fullscreen;
        bool _dirty;
        virtual void draw() = 0;
    public:
        /// returns window width
        inline int getWidth() { return _rect.width(); }
        /// returns window height
        inline int getHeight() { return _rect.height(); }
        /// sets dirty flag
        virtual void setDirty() { _dirty = true; }
        /// returns true if window is changed but now drawn
        virtual bool isDirty() { return _dirty; }
        /// shows or hides window
        virtual void setVisible( bool visible ) { _visible = visible; setDirty(); }
        virtual bool isVisible() const { return true; }
        virtual const lvRect & getRect() const { return _rect; }
        virtual void setRect( const lvRect & rc ) { _rect = rc; setDirty(); }
        virtual void flush() { draw(); _dirty = false; }
        /// returns true if window is fullscreen
        virtual bool isFullscreen() { return _fullscreen; }
        /// set fullscreen state for window
        virtual void setFullscreen( bool fullscreen ) { _fullscreen = fullscreen; }
        virtual CRGUIWindowManager * getWindowManager() { return _wm; }
        CRGUIWindowBase( CRGUIWindowManager * wm )
        : _wm(wm), _visible(true), _fullscreen(true), _dirty(true)
        {
            // fullscreen visible by default
            _rect = _wm->getScreen()->getRect();
        }
        virtual ~CRGUIWindowBase() { }
};

/// Base Screen class implementation
class CRGUIScreenBase : public CRGUIScreen
{
    protected:
        int _width;
        int _height;
        lvRect _updateRect;
        LVRef<LVDrawBuf> _canvas;
        LVRef<LVDrawBuf> _front;
        /// override in ancessor to transfer image to device
        virtual void update( const lvRect & rc, bool full ) = 0;
    public:
        /// creates compatible canvas of specified size
        virtual LVDrawBuf * createCanvas( int dx, int dy )
        {
#if (COLOR_BACKBUFFER==1)
            LVDrawBuf * buf = new LVColorDrawBuf( dx, dy );
#else
            LVDrawBuf * buf = new LVGrayDrawBuf( dx, dy, GRAY_BACKBUFFER_BITS );
#endif
            return buf;
        }
        /// sets new screen size
        virtual bool setSize( int dx, int dy )
        {
            if ( _width!=dx || _height != dy ) {
                _width = dx;
                _height = dy;
                _canvas = LVRef<LVDrawBuf>( createCanvas( dx, dy ) );
                if ( !_front.isNull() )
                    _front = LVRef<LVDrawBuf>( createCanvas( dx, dy ) );
                return true;
            }
            return false;
        }

        /// returns screen width
        virtual int getWidth() { return _width; }
        /// returns screen height
        virtual int getHeight() { return _height; }
        /// return pointer to screen canvas
        virtual LVRef<LVDrawBuf> getCanvas() { return _canvas; }
        /// draw image on screen canvas
        virtual void draw( LVDrawBuf * img, int x = 0, int y = 0)
        {
            img->DrawTo( _canvas.get(), x, y, 0, NULL );
        }
        /// transfers contents of buffer to device, if full==true, redraws whole screen, otherwise only changed area
        virtual void update( bool full )
        {
            if ( _updateRect.isEmpty() && !full )
                return;
            if ( !_front.isNull() && !_updateRect.isEmpty() && !full ) {
                // calculate really changed area
                lvRect rc;
                lvRect lineRect(_updateRect);
                int sz = _width * _canvas->GetBitsPerPixel() / 8;
                for ( int y = _updateRect.top; y < _updateRect.bottom; y++ ) {
                    if ( y>=0 && y<_height ) {
                        void * line1 = _canvas->GetScanLine( y );
                        void * line2 = _front->GetScanLine( y );
                        if ( memcmp( line1, line2, sz ) ) {
                            // line content is different
                            lineRect.top = y;
                            lineRect.bottom = y+1;
                            rc.extend( lineRect );
                            // copy line to front buffer
                            memcpy( line2, line1, sz );
                        }
                    }
                }
                if ( rc.isEmpty() ) {
                    // no actual changes
                    _updateRect.clear();
                    return;
                }
                _updateRect.top = rc.top;
                _updateRect.bottom = rc.bottom;
            }
            if ( full && !_front.isNull() ) {
                // copy full screen to front buffer
                _canvas->DrawTo( _front.get(), 0, 0, 0, NULL );
            }
            if ( full )
                _updateRect = getRect();
            update( _updateRect, full );
            _updateRect.clear();
        }
        /// invalidates rectangle: add it to bounding box of next partial update
        virtual void invalidateRect( const lvRect & rc )
        {
            _updateRect.extend( rc );
        }
        CRGUIScreenBase( int width, int height, bool doublebuffer  )
        : _width( width ), _height( height ), _canvas(NULL), _front(NULL)
        {
            _canvas = LVRef<LVDrawBuf>( createCanvas( width, height ) );
            if ( doublebuffer )
                _front = LVRef<LVDrawBuf>( createCanvas( width, height ) );
        }
        virtual ~CRGUIScreenBase()
        {
        }
};

#ifdef CR_WX_SUPPORT
/// WXWidget support: draw to wxImage
class CRWxScreen : public CRGUIScreenBase
{
    protected:
        wxBitmap _wxbitmap;
        virtual void update( const lvRect & rc, bool full )
        {
            wxImage img;
            int dyy = _canvas->GetHeight();
            int dxx = _canvas->GetWidth();
            int dx = dxx;
            int dy = dyy;
            img.Create(dx, dy, true);
            unsigned char * bits = img.GetData();
            for ( int y=0; y<dy && y<dyy; y++ ) {
                int bpp = _canvas->GetBitsPerPixel();
                if ( bpp==32 ) {
                    const lUInt32* src = (const lUInt32*) _canvas->GetScanLine( y );
                    unsigned char * dst = bits + y*dx*3;
                    for ( int x=0; x<dx && x<dxx; x++ )
                    {
                        lUInt32 c = *src++;
                        *dst++ = (c>>16) & 255;
                        *dst++ = (c>>8) & 255;
                        *dst++ = (c>>0) & 255;
                    }
                } else if ( bpp==2 ) {
                    //
                    static const unsigned char palette[4][3] = {
                        { 0xff, 0xff, 0xff },
                        { 0xaa, 0xaa, 0xaa },
                        { 0x55, 0x55, 0x55 },
                        { 0x00, 0x00, 0x00 },
                    };
                    const lUInt8* src = (const lUInt8*) _canvas->GetScanLine( y );
                    unsigned char * dst = bits + y*dx*3;
                    for ( int x=0; x<dx && x<dxx; x++ )
                    {
                        lUInt32 c = (( src[x>>2] >> ((3-(x&3))<<1) ))&3;
                        *dst++ = palette[c][0];
                        *dst++ = palette[c][1];
                        *dst++ = palette[c][2];
                    }
                } else if ( bpp==1 ) {
                    //
                    static const unsigned char palette[2][3] = {
                        { 0xff, 0xff, 0xff },
                        { 0x00, 0x00, 0x00 },
                    };
                    const lUInt8* src = (const lUInt8*) _canvas->GetScanLine( y );
                    unsigned char * dst = bits + y*dx*3;
                    for ( int x=0; x<dx && x<dxx; x++ )
                    {
                        lUInt32 c = (( src[x>>3] >> ((7-(x&7))) ))&1;
                        *dst++ = palette[c][0];
                        *dst++ = palette[c][1];
                        *dst++ = palette[c][2];
                    }
                }
            }

            // copy to bitmap
            wxBitmap bmp( img );
            _wxbitmap = bmp;
        }
    public:
        CRWxScreen( int width, int height )
        :  CRGUIScreenBase( width, height, true ) { }
        wxBitmap getWxBitmap() { return _wxbitmap; }
};
#endif

class CRDocViewWindow : public CRGUIWindowBase
{
    protected:
        LVDocView * _docview;
        virtual void draw()
        {
            LVDocImageRef pageImage = _docview->getPageImage(0);
            LVDrawBuf * drawbuf = pageImage->getDrawBuf();
            _wm->getScreen()->draw( drawbuf, _rect.left, _rect.top );
        }
    public:
        LVDocView * getDocView()
        {
            return _docview;
        }
        CRDocViewWindow( CRGUIWindowManager * wm )
        : CRGUIWindowBase( wm )
        {
            _docview = new LVDocView();
            _docview->Resize( getWidth(), getHeight() );
        }
        virtual ~CRDocViewWindow()
        {
            delete _docview;
        }
        virtual void setRect( const lvRect & rc )
        {
            if ( rc == _rect )
                return;
            _rect = rc;
            _docview->Resize( getWidth(), getHeight() );
            setDirty();
        }
        /// returns true if command is processed
        virtual bool onCommand( int command, int params )
        {
            if ( command >= LVDOCVIEW_COMMANDS_START && command <= LVDOCVIEW_COMMANDS_END ) {
                _docview->doCommand( (LVDocCmd)command, params );
                _dirty = true;
                return true;
            }
            return false;
        }
        /// returns true if window is changed but now drawn
        virtual bool isDirty()
        {
            return _dirty || !_docview->IsRendered() || !_docview->IsDrawed();
        }
};
