#!/bin/sh

function doUpdate {
	rm -rf pb$1/cr3gui
	bash makepb$1.sh
	bash makepb$1.sh
}

if [ "$1" = "" ]; then
	for TYPE in '360' 'pro4' 'pro5'; do
		doUpdate "$TYPE"
	done
else
	doUpdate "$1"
fi
