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
	convert "$SRC_DIR/$1" -resize 50% -channel RGBA -depth 24 "$2"
	mv "$2" "${2}.hd"
	convert "$SRC_DIR/$1" -resize 25% -channel RGBA -depth 24 "$2"
}

find * \( -type f -or -type l \) | while read FILE
do
	if [ "$FILE" -nt "$DEST_DIR/$FILE" ]; then
		mkdir -p "$(dirname "$DEST_DIR/$FILE")"
		if    [[ -f "$FILE" ]] && [[ "${FILE#*.}" == "png" ]]; then
			downscalepng "$FILE" "$DEST_DIR/$FILE"
		elif  [[ -f "$FILE" ]] && [[ "${FILE#*.}" == "lua" ]]; then
			compresslua "$FILE" "$DEST_DIR/$FILE"
		else
			cp -a "$FILE" "$DEST_DIR/$FILE"
			echo "Copying file \"$FILE\""
		fi
	fi
done
