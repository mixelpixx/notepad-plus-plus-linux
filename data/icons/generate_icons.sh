#!/bin/bash
# Regenerates all hicolor icon sizes from the master SVG logo.
# Requires: imagemagick (sudo apt install imagemagick)
#
# Usage: ./generate_icons.sh [path/to/source.svg]
#   Default source: ../../resources/images/notepad++linux.svg

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_FILE="${1:-$SCRIPT_DIR/../../resources/images/notepad++linux.svg}"
ICON_NAME="notepadplusplus"
BASE_DIR="$SCRIPT_DIR/hicolor"

if ! command -v convert &>/dev/null; then
    echo "Error: ImageMagick (convert) not found. Install with: sudo apt install imagemagick"
    exit 1
fi

if [ ! -f "$SOURCE_FILE" ]; then
    echo "Error: Source SVG '$SOURCE_FILE' not found."
    exit 1
fi

echo "Source: $SOURCE_FILE"

# Copy SVG to scalable/apps (transparent, vector, no rasterization needed)
mkdir -p "$BASE_DIR/scalable/apps"
cp "$SOURCE_FILE" "$BASE_DIR/scalable/apps/${ICON_NAME}.svg"
echo "  Copied:    $BASE_DIR/scalable/apps/${ICON_NAME}.svg"

# Generate rasterized PNGs for each standard hicolor size
# -background none preserves SVG transparency
for size in 16 22 24 32 48 64 96 128 256; do
    TARGET="$BASE_DIR/${size}x${size}/apps/${ICON_NAME}.png"
    mkdir -p "$(dirname "$TARGET")"
    convert -background none "$SOURCE_FILE" -resize ${size}x${size} "$TARGET"
    echo "  Generated: $TARGET"
done

echo "Done. Commit the generated files to include them in the repository."
