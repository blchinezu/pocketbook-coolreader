#!/bin/sh

#
# This file is used by the OTA Update feature.
# If you're installing manually just ignore this file.
# 
# You shouldn't launch it manually.
# Anyway, if you really want to:
#  - Put the archive contents in /mnt/ext1/.cr3update
#  - Launch the script from terminal
#

WORK_DIR="/mnt/ext1"
TEMP_DIR="$WORK_DIR/.cr3update"
UPDATE_PACKAGE="$WORK_DIR/latest.zip"
UPDATE_MARKER="$WORK_DIR/system/.cr3updated"

if [ ! -d "$WORK_DIR" ]; then
	echo "Your directory structure is unknown!"
	exit
fi

if [ ! -f "$TEMP_DIR/install.sh" ]; then
	echo "You probably launched this manually."
	echo "You shouldn't!"
	exit
fi

# cd to working dir
cd "$WORK_DIR"

# update launcher
/bin/rm -rf "$WORK_DIR/system/bin/cr3-pb.app"
/bin/mv "$TEMP_DIR/system/bin/cr3-pb.app" "$WORK_DIR/system/bin/cr3-pb.app"

# update share
/bin/rm -rf "$WORK_DIR/system/share/cr3"
/bin/mv "$TEMP_DIR/system/share/cr3" "$WORK_DIR/system/share/cr3"

# remove temporary dir and update package
/bin/rm -rf "$TEMP_DIR"
/bin/rm -rf "$UPDATE_PACKAGE"

# Mark update
echo "" > "$UPDATE_MARKER"

# relaunch coolreader
exec /mnt/ext1/system/bin/cr3-pb.app "$1" "updated" &
