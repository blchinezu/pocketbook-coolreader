#!/bin/sh

sdk="$HOME/PBDEV/sources/cr3-fork"
package="/tmp/cr3-package-$1"
releases="$HOME/PBDEV/releases/coolreader3"

cd "$sdk"

VERSION="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_VERSION | awk '{print $3}' | sed -e s/\\\"//g`"
DATE="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_BUILD_DATE | awk '{print $3}' | sed -e s/\\\"//g`"

function doUpdate {

	sdk="$HOME/PBDEV/sources/cr3-fork"
	package="/tmp/cr3-package-$1"
	releases="$HOME/PBDEV/releases/coolreader3"

	cd "$sdk"

	VERSION="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_VERSION | awk '{print $3}' | sed -e s/\\\"//g`"
	DATE="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_BUILD_DATE | awk '{print $3}' | sed -e s/\\\"//g`"

	echo
	echo "Version: $VERSION / $DATE / $1 / $2"

	if [ -f $sdk/pb$1/cr3gui/cr3-pb.app ]; then

		rm -rf "$package"
		mkdir -p "$package"

		echo
		echo "Update dev package $1"

		if [ ! -f $sdk/pb$1/cr3gui/cr3-pb.app ]; then
			echo 
			echo "   ERR: Binary not found!"
			echo 
			exit
		fi
		if [ "`ls $sdk/pb$1/cr3gui/*.cr3skin`" = "" ]; then
			echo 
			echo "   ERR: Skins not found!"
			echo 
			exit
		fi
		if [ "`ls $sdk/pb$1/i18n`" = "" ]; then
			echo 
			echo "   ERR: Translations not found!"
			echo 
			exit
		fi
		
		echo " - translations > common"
		rm -rf $package/system/share/cr3/i18n/*
		ls $sdk/pb$1/i18n | xargs -I lang cp -f "$sdk/pb$1/i18n/lang/LC_MESSAGES/cr3.mo" "$sdk/data/common/system/share/cr3/i18n/lang.mo"

		echo
		echo " - common > package"
		cp -rf $sdk/data/common/* $package/

		if [ -d "$sdk/data/firmware-specific/$1" ]; then
			echo
			echo " - FIRMWARE $1 specific > package"
			cp -rf $sdk/data/firmware-specific/$1/* $package/
		fi

		if [ "$2" != "" -a "$2" != "publish" -a -d "$sdk/data/device-specific/$2" ]; then
			echo
			echo " - DEVICE $2 specific > package"
			cp -rf $sdk/data/device-specific/$2/* $package/
		fi

		echo " - binary > package"
		rm -rf $package/system/share/cr3/bin/*
		mkdir -p $package/system/share/cr3/bin
		cp -f $sdk/pb$1/cr3gui/cr3-pb.app $package/system/share/cr3/bin/

		echo " - skins > package"
		rm -rf $package/system/share/cr3/skins/*
		# if [ "$1" = "360" ]; then
			cp -f $sdk/pb$1/cr3gui/default.cr3skin $package/system/share/cr3/skins/
		# fi
		if [ "$1" = "pro2" -o "$1" = "pro4" -o "$1" = "pro5" ]; then
			cp -f $sdk/pb$1/cr3gui/pb62x.cr3skin $package/system/share/cr3/skins/
		fi
		if [ "$1" = "pro4" -o "$1" = "pro5" ]; then
			cp -f $sdk/pb$1/cr3gui/pb626fw5.cr3skin $package/system/share/cr3/skins/
		fi

		echo " - package.version > package"
		if [ "$2" = "" ]; then
			echo "Version: $VERSION" > "$package/package.version"
			echo "Date:    `date +"%Y-%m-%d %H:%M:%S"`" >> "$package/package.version"
			echo "Build:   $1" >> "$package/package.version"
			echo "Branch:  dev" >> "$package/package.version"
		else
			echo "Version: $VERSION" > "$package/package.version"
			echo "Date:    $DATE" >> "$package/package.version"
			echo "Build:   $1" >> "$package/package.version"
			echo "Branch:  master" >> "$package/package.version"
		fi

		echo " - package > zip"
		zip="$releases/dev/cr3-v$VERSION-$1.zip"
		rm -f "$zip"
		cd "$package"
		zip -r "$zip" ./*
		cd "$sdk"
		if [ ! -f "$zip" ]; then
			echo
			echo "ERR: Couldn't create archive!"
			echo
			exit
		fi

		if [ "$2" = "publish" ]; then
			echo " - dev > stable (dropbox)"
			cp "$zip" "$releases/cr3-v$VERSION-$1.zip"
		fi

		if [ "$2" != "" -a "$2" != "publish" ]; then
			echo " - dev > builds (git)"
			rm -rf $sdk/builds/$2/$1/*
			mkdir -p "$sdk/builds/$2/$1"
			printf "yes" > "$sdk/builds/$2/$1/exists"
			cp -f "$zip" "$sdk/builds/$2/$1/latest.zip"
		fi

		if [ "$2" = "" -a "$1" = "pro5" ]; then
			old="`cat $sdk/cr3gui/po/cr3.pot | grep -v '#:' | grep -v '#, c-format' | grep -v 'POT-Creation-Date' | sort | uniq`"
			new="`cat $sdk/pbpro5/cr3gui/po/cr3.pot | grep -v '#:' | grep -v '#, c-format' | grep -v 'POT-Creation-Date' | sort | uniq`"
			diff="`diff  <(echo "$old" ) <(echo "$new")`"

			if [ "$diff" != "" ]; then
				echo " - update i18n template"
				rm -f "$sdk/cr3gui/po/cr3.pot"
				cat "$sdk/pbpro5/cr3gui/po/cr3.pot" | grep -v '#, c-format' > "$sdk/cr3gui/po/cr3.pot"
				cd "$sdk/cr3gui/po"
				bash update-po
				cd "$sdk"
			fi
		fi

		if [ "$2" = "" -a "$1" = "pro5" -a -f "$releases/dev/cr3-v$VERSION-$1.zip" ]; then
			echo
			echo "Update git dev branch?"
			select yn in "Yes" "No"; do
			    case $yn in
			        Yes )
						echo
						echo " - DEV: Update git build: $1"
						rm -f "builds/626/$1/latest.zip"
						cp "$releases/dev/cr3-v$VERSION-$1.zip" "builds/626/$1/latest.zip"
						git commit "builds/626/$1/latest.zip" -m 'Auto update dev build [publish.sh]'
						sleep 1s
						git push
						break;;
			        No )
						break;;
			    esac
			done
		fi
	fi
}

# Update all packages
if [ "$1" = "" ]; then
	for TYPE in '360' 'pro2' 'pro4' 'pro5'; do
		doUpdate "$TYPE"
	done

# Update all builds and publish
elif [ "$1" = "publish" ]; then

	printf "$VERSION" > "$sdk/builds/current.version"

	echo

	# Check firmware specific
	for FIRMWARE in '360' 'pro2' 'pro4' 'pro5'; do
		
		# Check if it's already published
		echo " - Check if published:    $FIRMWARE"
		if [ -f $releases/cr3-v$VERSION-$FIRMWARE.zip ]; then
			echo "   ERR: Version already published!"
			exit
		fi

		# Check if there's no binary
		echo " - Check firmware binary: $FIRMWARE"
		if [ ! -f $sdk/pb$FIRMWARE/cr3gui/cr3-pb.app ]; then
			echo "   ERR: No binary found!"
			exit
		fi
		echo
	done

	# Move old builds
	echo " - Move old builds"
	mv $releases/*.zip $releases/old/

	# Publish firmware specific (dropbox)
	for TYPE in '360' 'pro2' 'pro4' 'pro5'; do
		doUpdate "$TYPE" "publish"
	done

	# Publish device specific (git)
	for DEVICE in '360'; do
		for FIRMWARE in '360'; do
			doUpdate "$FIRMWARE" "$DEVICE"
		done
	done
	for DEVICE in '602'; do
		for FIRMWARE in 'pro2'; do
			doUpdate "$FIRMWARE" "$DEVICE"
		done
	done
	for DEVICE in '515' '626'; do
		for FIRMWARE in 'pro4' 'pro5'; do
			doUpdate "$FIRMWARE" "$DEVICE"
		done
	done
	
	# Wait for dropbox to catch on
	echo
	echo " - Wait for dropbox to catch on..."
	sleep 2s
	while [ "`dropbox status|grep "Up to date"`" = "" ]; do
		sleep 1s
	done

	# RELOAD DROPBOX
	sleep 1s
	echo " - Restart dropbox"
	dropbox stop
	sleep 1s
	dropbox start

	# COMMIT GIT
	cd "$sdk"
	echo " - Commit to github"
	git commit -a -m "Up version: $VERSION"
	sleep 1s

	echo " - Push to web"
	git push


# Update a package
else
	doUpdate "$1"
fi

echo
