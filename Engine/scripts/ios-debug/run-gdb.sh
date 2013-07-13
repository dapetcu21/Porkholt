#!/bin/bash
set -e

LOCAL_FILE=$1
REMOTE_FILE=$2
DSYM_BUNDLE=$3
LOCAL_CONTAINER="$(dirname "${LOCAL_FILE}")"
REMOTE_CONTAINER="$(dirname "${REMOTE_FILE}")"

TEMP=$(mktemp -d /tmp/gdbios.XXXX)

scp root@${PH_DEVICE_IP}:/System/Library/CoreServices/SystemVersion.plist "${TEMP}/sys.plist" > /dev/null
OS_VERSION=$(/usr/libexec/PlistBuddy -c "Print :ProductVersion" "${TEMP}/sys.plist")
BUILD_VERSION=$(/usr/libexec/PlistBuddy -c "Print :ProductBuildVersion" "${TEMP}/sys.plist")

DS_ROOTS=("/Developer/Platforms/iPhoneOS.platform/DeviceSupport" "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/DeviceSupport" "${HOME}/Library/Developer/Xcode/iOS DeviceSupport")
for i in "${DS_ROOTS[@]}"; do 
    _DS_PATH="${i}/${OS_VERSION} (${BUILD_VERSION})"
    if [ -d "${_DS_PATH}" ]; then
        DS_PATH="${_DS_PATH}"
        break;
    fi
done
if [ -z "${DS_PATH}" ]; then
    echo "Can't find proper debug symbols. Please connect your device to Xcode and let it do its thing"
    exit 1
fi

if [ ! -z "${DSYM_BUNDLE}" ]; then
    GDB_CMDS="add-symbol-file \"${DSYM_BUNDLE}\"
    "
    if [ "${LOCAL_FILE}" -nt "${DSYM_BUNDLE}" ]; then
        echo "Generating dSYM bundle"
        dsymutil "--out=${DSYM_BUNDLE}" "${LOCAL_FILE}"
    fi
fi

GDB_CMDS="${GDB_CMDS}set mi-show-protections off
set auto-raise-load-levels 1
set shlib-path-substitutions /usr \"${DS_PATH}/Symbols/usr\" /System \"${DS_PATH}/Symbols/System\" \"${REMOTE_CONTAINER}\" \"${LOCAL_CONTAINER}\" \"/private${REMOTE_CONTAINER}\" \"${LOCAL_CONTAINER}\" /Developer \"${DS_PATH}/Symbols/Developer\"
set remote max-packet-size 1024
set sharedlibrary check-uuids on
set env NSUnbufferedIO YES
set minimal-signal-handling 1
set sharedlibrary load-rules \\\".*\\\" \\\".*\\\" container
set inferior-auto-start-dyld 0
file \"${LOCAL_FILE}\"
set remote executable-directory $(dirname "${REMOTE_FILE}")
set remote noack-mode 1
set trust-readonly-sections 1
target remote-macosx ${PH_DEVICE_IP}:6789
mem 0x1000 0x3fffffff cache
mem 0x40000000 0xffffffff none
mem 0x00000000 0x0fff none
run
set minimal-signal-handling 0
set inferior-auto-start-cfm off
set sharedLibrary load-rules dyld \".*libobjc.*\" all dyld \".*CoreFoundation.*\" all dyld \".*Foundation.*\" all dyld \".*libSystem.*\" all dyld \".*AppKit.*\" all dyld \".*PBGDBIntrospectionSupport.*\" all dyld \".*/usr/lib/dyld.*\" all dyld \".*CarbonDataFormatters.*\" all dyld \".*libauto.*\" all dyld \".*CFDataFormatters.*\" all dyld \"/System/Library/Frameworks\\\\\\\\|/System/Library/PrivateFrameworks\\\\\\\\|/usr/lib\" extern dyld \".*\" all exec \".*\" all
sharedlibrary apply-load-rules all
set inferior-auto-start-dyld 1"

echo "${GDB_CMDS}" > "${TEMP}/gdbinit"
ssh -f root@${PH_DEVICE_IP} debugserver localhost:6789
xcrun -sdk iphoneos gdb -arch arm -q -x "${TEMP}/gdbinit"
rm -rf "${TEMP}"

