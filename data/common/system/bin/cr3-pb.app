#!/bin/sh

WORK_DIR="/mnt/ext1"
RESTART_MARK="$WORK_DIR/.cr3restart"
UPDATE_PACKAGE="$WORK_DIR/latest.zip"
TEMP_DIR="$WORK_DIR/.cr3update"
INSTALLER="$TEMP_DIR/install.sh"

# cd to working dir
cd "$WORK_DIR"

# remove temporary dir if present
if [ -d "$TEMP_DIR" ]; then
    rm -rf "$TEMP_DIR"
fi

# if there's an update package
if [ -f "$UPDATE_PACKAGE" ]; then

    # create temp dir
    mkdir -p "$TEMP_DIR"

    # unzip package
    unzip "$UPDATE_PACKAGE" -d "$TEMP_DIR"

    # remove the update package
    rm -f "$UPDATE_PACKAGE"

    # if the update package has been removed and there's an installer
    if [ -f "$INSTALLER" -a ! -f "$UPDATE_PACKAGE" ]; then

        # full backup. it'll be done at some point

        # launch installer
        sh "$INSTALLER" "$1"

        # stop script. it's relaunched from the installer.
        exit
    fi
fi

# keep starting time
startedTime="`date +"%s"`"

# launch cr3
exec $WORK_DIR/system/share/cr3/bin/cr3-pb.app "$1" > $WORK_DIR/system/cr3-pb.normal.log 2> $WORK_DIR/system/cr3-pb.crash.log

# stopped time
stoppedTime="`date +"%s"`"

# if restart required
if [ -f "$RESTART_MARK" ]; then

    # remove marker
    rm -f "$RESTART_MARK"

    # relaunch this script
    $WORK_DIR/system/bin/cr3-pb.app "$1" &

    # stop script
    exit
fi

# try detecting crash (less than 5 seconds of running time)
runTime=$(($stoppedTime-$startedTime))
if [ $runTime -lt 5 ]; then

    # Restore backup if needed (will be done at some point)
fi

