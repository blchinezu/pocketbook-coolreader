#!/bin/sh

function doUpdate {
	rm -rf pb$1/cr3gui
	bash make.sh $1
}

if [ "$1" = "" ]; then
	for TYPE in '360' 'pro2' 'pro4' 'pro5'; do
		doUpdate "$TYPE"
	done
else
	doUpdate "$1"
fi
