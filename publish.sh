#!/bin/sh

SVN="/media/Fast/dev/svn/brucelee.duckdns.org/cr3-pb-ota"

VERSION="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_VERSION | awk '{print $3}' | sed -e s/\\\"//g`"
DATE="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_BUILD_DATE | awk '{print $3}' | sed -e s/\\\"//g`"
if [ "$VERSION" != "`cat "$VERSION_FILE"`" ]; then
	printf "$VERSION" > "$SVN/current.version"
	printf "$VERSION" > "builds/current.version"
fi

echo
echo "Publishing: $VERSION / $DATE"
echo

sdk="$HOME/PBDEV/sources/cr3-fork"
releases="$HOME/PBDEV/releases/coolreader3"

if [ -f "$releases/cr3-v$VERSION-pro5.zip" ]; then
	echo " -> Version already published!"
	exit
fi

echo " - Move old builds"
mv $releases/*.zip $releases/old/

for TYPE in '360' 'pro4' 'pro5'; do
	echo " - Basic: $1"
	zip -r "$releases/cr3-v$VERSION-$1.zip" "$releases/dev/cr3-$1/system"
done

echo " - Stop dropbox"
dropbox stop

echo " - Wait 5 seconds..."
sleep 5s

echo " - Start dropbox"
dropbox start
