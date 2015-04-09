#!/bin/sh

VERSION="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_VERSION | awk '{print $3}' | sed -e s/\\\"//g`"
DATE="`cat cr3gui/src/cr3pocketbook.h | grep CR_PB_BUILD_DATE | awk '{print $3}' | sed -e s/\\\"//g`"

echo
echo "Version: $VERSION / $DATE"

function doUpdate {
	sdk="$HOME/PBDEV/sources/cr3-fork"
	release="$HOME/PBDEV/releases/coolreader3/dev"

	if [ -f $sdk/pb$1/cr3gui/cr3-pb.app ]; then

		echo
		echo "Update dev release $1"

		echo " - binary"
		if [ ! -f $sdk/pb$1/cr3gui/cr3-pb.app ]; then
			echo 
			echo "   ERR: Binary not found!"
			echo 
			exit
		fi
		rm -rf $release/cr3-$1/system/share/cr3/bin/*
		mkdir -p $release/cr3-$1/system/share/cr3/bin
		cp -f $sdk/pb$1/cr3gui/cr3-pb.app $release/cr3-$1/system/share/cr3/bin/

		echo " - skins"
		if [ "`ls $release/cr3-$1/system/share/cr3/skins/`" = "" ]; then
			echo 
			echo "   ERR: Skins not found!"
			echo 
			exit
		fi
		rm -rf $release/cr3-$1/system/share/cr3/skins/*
		# if [ "$1" = "360" ]; then
			cp -f $sdk/pb$1/cr3gui/default.cr3skin $release/cr3-$1/system/share/cr3/skins/
		# fi
		if [ "$1" = "602" -o "$1" = "pro4" -o "$1" = "pro5" ]; then
			cp -f $sdk/pb$1/cr3gui/pb62x.cr3skin $release/cr3-$1/system/share/cr3/skins/
		fi
		if [ "$1" = "pro4" -o "$1" = "pro5" ]; then
			cp -f $sdk/pb$1/cr3gui/pb626fw5.cr3skin $release/cr3-$1/system/share/cr3/skins/
		fi
		
		echo " - translations"
		if [ "`ls $sdk/pb$1/i18n`" = "" ]; then
			echo 
			echo "   ERR: Translations not found!"
			echo 
			exit
		fi
		rm -rf $release/cr3-$1/system/share/cr3/i18n/*
		ls $sdk/pb$1/i18n | xargs -I lang cp -f "$sdk/pb$1/i18n/lang/LC_MESSAGES/cr3.mo" "$release/cr3-$1/system/share/cr3/i18n/lang.mo"

		bash publish.sh "dev" "$1"
	fi
}

if [ "$1" = "" ]; then
	for TYPE in '360' '602' 'pro4' 'pro5'; do
		doUpdate "$TYPE"
	done
else
	doUpdate "$1"
fi
echo
