#!/bin/sh

SVN="/media/Fast/dev/svn/brucelee.duckdns.org/cr3-pb-ota"

# MARK VERSION
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

# CHECK IF IT'S ALREADY PUBLISHED
if [ -f "$releases/cr3-v$VERSION-pro5.zip" ]; then
	echo " -> Version already published!"
	exit
fi

# MOVE OLD BUILDS
echo " - Move old builds"
mv $releases/*.zip $releases/old/

# CHECK FIRMWARE SPECIFIC
for FIRMWARE in '360' 'pro4' 'pro5'; do
	echo " - Check firmware binary: $1"
	if [ ! -f $releases/dev/cr3-$FIRMWARE/system/share/cr3/bin/cr3-pb.app ]; then
		echo "   ERR: No binary found!"
		exit
	fi
done

# BUILD FIRMWARE SPECIFIC - DROPBOX
for FIRMWARE in '360' 'pro4' 'pro5'; do
	echo " - Firmware specific: $FIRMWARE"
	cd $releases/dev/cr3-$FIRMWARE/
	zip -r "$releases/cr3-v$VERSION-$FIRMWARE.zip" ./*
done

function doPublish {
	device="$1"
	fw="$2"
	sdk="$3"
	releases="$4"

	tmpDir="/tmp/cr3Publish/$device/$fw"

	echo " - Device specific: $device / $fw"
	rm -rf "$tmpDir"
	mkdir -p "$tmpDir"

	# copy main things
	cp -r $releases/dev/cr3-$fw/* $tmpDir/

	# copy exceptions
	rm -f $tmpDir/system/share/cr3/keymaps/*
	cp $releases/dev/device-$device/system/share/cr3/keymaps/* $tmpDir/system/share/cr3/keymaps/

	rm -f $tmpDir/system/share/cr3/devices.ini
	if [ -f $releases/dev/device-$device/system/share/cr3/devices.ini ]; then
		cp $releases/dev/device-$device/system/share/cr3/devices.ini $tmpDir/system/share/cr3/
	fi

	# create package
	mkdir -p "$sdk/builds/$device/$fw"
	if [ -f "$sdk/builds/$device/$fw/latest.zip" ]; then
		rm -f "$sdk/builds/$device/$fw/latest.zip"
	fi
	cd $tmpDir
	zip -r "$sdk/builds/$device/$fw/latest.zip" ./*
	if [ -f "$sdk/builds/$device/$fw/latest.zip" ]; then
		printf "yes" > "$sdk/builds/$device/$fw/exists"
	else
		printf "no" > "$sdk/builds/$device/$fw/exists"
	fi
	rm -rf $tmpDir
}

# BUILD DEVICE SPECIFIC - GIT / OTA
for DEVICE in '360' '515' '626'; do
	if [ "$DEVICE" = "360" ]; then
		doPublish "360" "360" "$sdk" "$releases"
	else
		for FIRMWARE in 'pro4' 'pro5'; do
			doPublish "$DEVICE" "$FIRMWARE" "$sdk" "$releases"
		done
	fi
done

# RELOAD DROPBOX
echo " - Stop dropbox"
dropbox stop

echo " - Wait 2 seconds..."
sleep 5s

echo " - Start dropbox"
dropbox start

# COMMIT GIT
cd "$sdk"
echo " - Commit to github"
git commit -a -m 'publish: $VERSION'

echo " - Push to web"
git push

# DONE
echo "Done"
