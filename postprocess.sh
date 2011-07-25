#/bin/bash

if [ ! -d "$1" ]; then
	echo "\"$1\" does not exist"
	exit 1
fi

if [ ! -d "$2" ]; then
	echo "\"$2\" does not exist"
	exit 1
fi

ROOT_DIR="`cd \"$1\" 2>/dev/null && pwd`"
SRC_DIR="`cd \"$2\" 2>/dev/null && pwd`"
BUILD_ACTION=$3
DEST_DIR="$ROOT_DIR/_/rsrc"

#add your lua path here
PATH=${PATH}:/opt/local/bin:/opt/local/sbin

if [ "$BUILD_ACTION" = "clean" ]; then
	rm -rf "$ROOT_DIR/_"
	exit 0
fi

mkdir -p "$DEST_DIR"
touch -cm "$DEST_DIR"

cd "$SRC_DIR"

compresslua()
{
	echo "Compressing lua script \"$1\""
	cd "$ROOT_DIR/Externals/LuaSrcDiet"
	./LuaSrcDiet.lua "$SRC_DIR/$1" -o "$2" --maximum >> /dev/null
	cd - >> /dev/null
}

downscalepng()
{
	#todo
	echo "Downscaling png file \"$1\""
	CONVERT_FLAGS="-quality 5 -channel RGBA -depth 24"
	convert "$SRC_DIR/$1" $CONVERT_FLAGS -resize 50% "$2"
	mv "$2" "${2}.hd"
	convert "$SRC_DIR/$1" $CONVERT_FLAGS -resize 25% "$2"
}

find * \( -type f -or -type l \) | while read FILE
do
	if [ "$FILE" -nt "$DEST_DIR/$FILE" ]; then
		mkdir -p "$(dirname "$DEST_DIR/$FILE")"
		BASENAME="`basename "${FILE}"`"
		EXTENSION="${BASENAME#*.}"
		if    [[ -f "$FILE" ]] && [[ "$EXTENSION" == "png" ]]; then
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
