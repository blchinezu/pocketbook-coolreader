#!/bin/sh

function waitForDropboxToCatchOn {
	printf " - wait for dropbox to catch on: "
	for s in {5..1}; do
		printf "$s "
		sleep 1s
	done
	printf "\n"
}

function doUpdate {
	sdk="$HOME/PBDEV/sources/cr3-fork"
	release="$HOME/PBDEV/releases/coolreader3/dev"

	if [ -f $sdk/pb$1/cr3gui/cr3-pb.app ]; then

		waitForDropboxToCatchOn
		echo
		echo "Update dev release $1"

		echo " - remove binary, skins, translations"
		# binary
		rm -rf $release/cr3-$1/system/bin
		# skins
		rm -rf $release/cr3-$1/system/share/cr3/skins
		# translations
		rm -rf $release/cr3-$1/system/share/cr3/i18n

		waitForDropboxToCatchOn

		echo " - copy binary, skins, translations"
		# binary
		mkdir -p $release/cr3-$1/system/bin
		cp -f $sdk/pb$1/cr3gui/cr3-pb.app $release/cr3-$1/system/bin/
		# skins
		mkdir -p $release/cr3-$1/system/share/cr3/skins
		cp -f $sdk/pb$1/cr3gui/*.cr3skin $release/cr3-$1/system/share/cr3/skins/
		# translations
		mkdir -p $release/cr3-$1/system/share/cr3/i18n
		ls $sdk/pb$1/i18n | xargs -I lang cp -f "$sdk/pb$1/i18n/lang/LC_MESSAGES/cr3.mo" "$release/cr3-$1/system/share/cr3/i18n/lang.mo"
	fi
}

echo
if [ "$1" = "" ]; then
	for TYPE in '360' 'pro4' 'pro5'; do
		doUpdate "$TYPE"
	done
else
	doUpdate "$1"
fi
echo
