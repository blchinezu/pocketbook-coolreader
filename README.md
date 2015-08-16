# CoolReader 3 - free e-book reader
(c) Vadim Lopatin, 1998-2009  
All source codes (except thirdparty directory) are provided under the terms of GNU GPL license, version 2

--------------------------------------------------------------------------------

This is a **fork of** [pocketbook-port (sourceforge.net)](https://sourceforge.net/p/crengine/crengine/ci/pocketbook-port/tree/)

**Source code** is available at [github.com](https://github.com/blchinezu/pocketbook-coolreader)

The **forum thread** is at [mobileread.com](http://www.mobileread.com/forums/showthread.php?t=256917)

**Help translating** the app at [transifex.com](https://www.transifex.com/projects/p/coolreader-pocketbook/)

**Donate** through PayPal at [brucelee.duckdns.org/donation/cr3-pb](http://brucelee.duckdns.org/donation/cr3-pb/)

    Since I can't directly add the PayPal donation button here, I've created a simple page
    which has the Donate button.

--------------------------------------------------------------------------------

### Features summary (compared to [pkb](https://sourceforge.net/p/crengine/crengine/ci/pocketbook-port/tree/)'s version)

**General**

 - FW5 compatible [pal]
 - Google & Wikipedia buttons in the dictionary window
 - Set book cover as power off logo [pal]
 - Get hardware depth from device
 - Full screen update on book load
 - Fixed font selection bug in settings [pkb]
 - Cite toolbar is working with touch devices
 - Updated freetype to v2.6
 - Updated translations
 - Stand By Mode
 - More hyphenations
 - Control front light by vertically swiping in FW5
 - Turn pages by horizontally swiping

**Settings**

 - Formatting settings > Space expanding percent [xl743]
 - Display settings > Standby
 - Font settings > Font embolding [xl743]
 - Font settings > Tracking [xl743]
 - Style settings > * > Letter spacing [xl743]
 - Page settings > Status line settings > Time format
 - Clear cache
 - OTA Update (through Github)

**Dictionary**

 - Show when long-tapping a word [pkb]
 - Window doesn't jump around
 - Set current word as default in the search keyboard 

**Tap actions**

 - Toggle system panel
 - Open system panel (FW5 only)
 - Launch front light (FW4 only, FW5 uses system panel)
 - Toggle invert display
 - Toggle status bar
 - Tasks list
 - Lock device
 - OTA Update
 - Full screen update
 - Toggle Front Light (FW5 only)
 - Set bookmark (for current page)
 - Delete bookmark (for current page)
 - Enter standby mode

**Theme**

 - Changed the grey things to black
 - New FW5 skin (for 758x1024 screens)
 - Redesigned menu for touch devices with resolution greater than 600x800 using theme pb626fw5
 - Long tap the bookmark icon in the new menu to toggle current page bookmark

--------------------------------------------------------------------------------

### Directories:

    crengine   - CREngine (DOM/XML/CSS ebook rendering library) sources
    cr3gui     - CR3 with CR3GUI for e-ink devices sources
    thirdparty - third party libraries, to use if not found in system
    tinydict   - small library for .dict file format support
    tools      - miscellaneous configuration files

--------------------------------------------------------------------------------

### External dependencies: 

    common: zlib, libpng, libjpeg, freetype, libcurl
    cr3gui/xcb: libxcb, fontconfig
    cr3gui/nanoX: libnanoX

--------------------------------------------------------------------------------

### Build:

    bash make.sh [360|pro2|pro4|pro5]

--------------------------------------------------------------------------------

### SDK

[SDK for PRO devices with firmware 5](https://github.com/pocketbook-free/SDK_481)

[SDK for 360 and PRO devices](https://sourceforge.net/projects/pocketbook-free/files/PocketBook_Pro_SDK_Linux_1.1/)

    For 360 use the PBSDK dir from the archive
    For pro2 and pro4 use the FRSCSDK dir from the archive

    libinkview.so is in FRSCSDK/arm-none-linux-gnueabi/sysroot/usr/lib/
      For pro2 you have to rename it to libinkview.1.1a.so
        (a symlink is created by make.sh when needed)
      For pro4 you have to add libinkview.pb626.fw4.4.so from a FW4 device to the lib dir
        (also a symlink is created by make.sh when needed)

--------------------------------------------------------------------------------

### My development dir looks like this:

    ~/PBDEV/FRSCSDK/
    ~/PBDEV/PBSDK/
    ~/PBDEV/SDK_481/
    ~/PBDEV/sources/cr3-fork/
    ~/PBDEV/system/
    
    With this structure you should be able to build the source without any changes.

--------------------------------------------------------------------------------

### Screenshots:

![0.jpg](https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/screenshots/0.jpg)
![1.jpg](https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/screenshots/1.jpg)
![2.jpg](https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/screenshots/2.jpg)
![3.jpg](https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/screenshots/3.jpg)
![4.jpg](https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/screenshots/4.jpg)
![5.jpg](https://raw.githubusercontent.com/blchinezu/pocketbook-coolreader/master/screenshots/5.jpg)
