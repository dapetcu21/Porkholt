#!/bin/bash

if [ ! -d "$1" ]; then
	echo "\"$1\" does not exist"
	exit 1
fi

if [ ! -d "$4" ]; then
	echo "\"$4\" does not exist"
	exit 1
fi

DEST_DIR=$2
mkdir -p "$DEST_DIR"
touch -cm "$DEST_DIR"

SRC_DIR="`cd \"$1\" 2>/dev/null && pwd`"
DEST_DIR="`cd \"$2\" 2>/dev/null && pwd`"
BUILD_ACTION=$3
EXTERNALS_DIR="`cd \"$4\" 2>/dev/null && pwd`"

#echo "SRC_DIR ${SRC_DIR}"
#echo "DEST_DIR ${DEST_DIR}"
#echo "BUILD_ACTION ${BUILD_ACTION}"
#echo "EXTERNALS_DIR ${EXTERNALS_DIR}"

#add your lua path here
PATH=${PATH}:/opt/local/bin:/opt/local/sbin
if which gm &> /dev/null; then
    alias convert="gm convert"
fi

if [ "$BUILD_ACTION" = "clean" ]; then
	rm -rf "$DEST_DIR"
	exit 0
fi

cd "$DEST_DIR"
nothd()
{
	AFILE=`echo "$1" | sed "s/.hd$//"`
	if [[ -f $AFILE ]]; then
		return 1
	else
		return 0
	fi
}
find . | while read FILE
do
	if [[ -f "$FILE" ]] && [[ ! -f "$SRC_DIR/$FILE" ]] && nothd "$SRC_DIR/$FILE" ; then
		rm "$FILE"
	fi
	if [[ -d "$FILE" ]] && [[ ! -d "$SRC_DIR/$FILE" ]]; then
		rm -r "$FILE"
	fi
	if [[ -h "$FILE" ]] && [[ ! -h "$SRC_DIR/$FILE" ]]; then
		rm "$FILE"
	fi
done

cd "$SRC_DIR"

compresslua()
{
	echo "Compressing lua script \"$1\""
	cd "$EXTERNALS_DIR/LuaSrcDiet"
	../lua/src/lua ./LuaSrcDiet.lua "$SRC_DIR/$1" -o "$2" --maximum >> /dev/null
	cd - >> /dev/null
}

downscalepng()
{
	#todo
	echo "Downscaling png file \"$1\""
	CONVERT_FLAGS="-quality 5 -channel RGBA -depth 24 -colorspace RGB"
	if [[ "$BUILD_ACTION" == "build-nodownscale" ]]
	then
#	    convert "$SRC_DIR/$1" $CONVERT_FLAGS "png32:$2"
      cp "${SRC_DIR}/$1" "$2"
  else
	    convert "$SRC_DIR/$1" $CONVERT_FLAGS -resize 50% "png32:$2"
	    mv "$2" "${2}.hd"
	    convert "$SRC_DIR/$1" $CONVERT_FLAGS -resize 25% "png32:$2"
    fi
}

find * \( -type f -or -type l \) | while read FILE
do
	if [ "$FILE" -nt "$DEST_DIR/$FILE" ]; then
		mkdir -p "$(dirname "$DEST_DIR/$FILE")"
		BASENAME="`basename "${FILE}"`"
		EXTENSION="${BASENAME#*.}"
		if    [[ -f "$FILE" ]] &&( [[ "$EXTENSION" == "png" ]] || [[ "$EXTENSION" == "png.nmap" ]] ); then
			downscalepng "$FILE" "$DEST_DIR/$FILE"
		elif  [[ -f "$FILE" ]] && [[ "$EXTENSION" == "lua" ]]; then
			compresslua "$FILE" "$DEST_DIR/$FILE"
		else
			echo "Copying file \"$FILE\""
			rm -f "$DEST_DIR/$FILE"
			cp -af "$FILE" "$DEST_DIR/$FILE"
		fi
	fi
done

exit 0
