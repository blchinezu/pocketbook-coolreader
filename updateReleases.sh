#!/bin/sh

function doUpdate {
	sdk="$HOME/PBDEV/sources/cr3-fork"
	release="$HOME/PBDEV/releases/coolreader3/dev"

	echo "Update dev release $1"

	if [ -f $sdk/pb$1/cr3gui/cr3-pb.app ]; then
		rm -f $release/cr3-$1/system/bin/cr3-pb.app
		cp -f $sdk/pb$1/cr3gui/cr3-pb.app $release/cr3-$1/system/bin/

		rm -f $release/cr3-$1/system/share/cr3/skins/*.cr3skin
		cp -f $sdk/pb$1/cr3gui/*.cr3skin $release/cr3-$1/system/share/cr3/skins/
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
