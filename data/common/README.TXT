
This is a fork of:
  https://sourceforge.net/p/crengine/crengine/ci/pocketbook-port/tree/


Source code is available at github:
  https://github.com/blchinezu/pocketbook-coolreader


The forum thread is at mobileread.com:
  http://www.mobileread.com/forums/showthread.php?t=256917


Help translating the app:
  https://www.transifex.com/projects/p/coolreader-pocketbook/


Package structure:

  /README
    This file

  /install.sh
    Used by CR3 for automatic updates (OTA Update)
    It's called by CR3 if it finds a "latest.zip" package in the device.

  /package.version
    Keeps package info: version, date, build, branch

  /system/config/extensions.cfg
    It's an example of extensions.cfg that will activate CR3

  /system/config/cr3/
    It's the folder that keeps the CR3 configuration.
    It's automatically created so there's no need to copy it to device.

  /system/bin/
    Must be copied to device (contains the launcher)

  /system/share/
    Must be copied to device (contains all the CR3 required files)


Package types:
  
  360  - Use it for 360 devices
  pro2 - Use it for devices having firmware 2
  pro4 - Use it for devices having firmware 4
  pro5 - Use it for devices having firmware 5
