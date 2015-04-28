CoolReader 3 - free e-book reader
(c) Vadim Lopatin, 1998-2009


LICENSE:
  All source codes (except thirdparty directory) 
  are provided under the terms of GNU GPL license, version 2


This is a fork of:
  https://sourceforge.net/p/crengine/crengine/ci/pocketbook-port/tree/


Source code is available at github:
  https://github.com/blchinezu/pocketbook-coolreader


The forum thread is at mobileread.com:
  http://www.mobileread.com/forums/showthread.php?t=256917


Help translating the app:
  https://www.transifex.com/projects/p/coolreader-pocketbook/


Directories:
  crengine   - CREngine (DOM/XML/CSS ebook rendering library) sources
  cr3gui     - CR3 with CR3GUI for e-ink devices sources
  thirdparty - third party libraries, to use if not found in system
  tinydict   - small library for .dict file format support
  tools      - miscellaneous configuration files


External dependencies: 
  common: zlib, libpng, libjpeg, freetype, libcurl
  cr3gui/xcb: libxcb, fontconfig
  cr3gui/nanoX: libnanoX


Build:
  bash make.sh [360|pro2|pro4|pro5]


SDK for 360 and PRO devices (including pro2):
  https://sourceforge.net/projects/pocketbook-free/files/PocketBook_Pro_SDK_Linux_1.1/
  For 360 use the PBSDK dir from the archive
  For pro2 and pro4 use the FRSCSDK dir from the archive

  libinkview.so is in FRSCSDK/arm-none-linux-gnueabi/sysroot/usr/lib/
    For pro2 you have to rename it to libinkview.1.1a.so
      (a symlink is created by make.sh when needed)
    For pro4 you have to add libinkview.pb626.fw4.4.so from a FW4 device to the lib dir
      (also a symlink is created by make.sh when needed)

  
SDK for PRO devices with firmware 5:
  https://github.com/pocketbook-free/SDK_481


My development dir looks like this:
  ~/PBDEV/FRSCSDK/
  ~/PBDEV/PBSDK/
  ~/PBDEV/SDK_481/
  ~/PBDEV/sources/cr3-fork/
  ~/PBDEV/system/
With this structure you should be able to build the source without any changes.
