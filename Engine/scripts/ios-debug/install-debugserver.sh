#!/bin/bash
set -e

if [ "$1" = "-?" ]; then
    echo "Usage: install-debugserver.sh <codesign identity>"
    exit 1
fi

if [ -z $1 ] ; then
    IDENTITY="iPhone Developer"
else
    IDENTITY=$1
fi

mkdir -p /tmp/gdbios
scp root@${PH_DEVICE_IP}:/Developer/usr/bin/debugserver /tmp/gdbios/debugserver
xcrun -sdk iphoneos codesign -f -s "${IDENTITY}" --entitlements ent.xml /tmp/gdbios/debugserver
scp /tmp/gdbios/debugserver root@${PH_DEVICE_IP}:/usr/bin/
rm -rf /tmp/gdbios
