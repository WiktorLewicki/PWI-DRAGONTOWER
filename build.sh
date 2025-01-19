#!/bin/bash
SCRIPT_DIR=$(dirname "$(realpath "$0")")
cd "$SCRIPT_DIR" || exit 1
rm -rf "$SCRIPT_DIR/Makefile" "$SCRIPT_DIR/*.o" "$SCRIPT_DIR/*.pro.user" "$SCRIPT_DIR/*.pro.*"
/usr/bin/qmake "$SCRIPT_DIR/src/DragonTower.pro" -o "$SCRIPT_DIR/Makefile"
make
./DragonTower