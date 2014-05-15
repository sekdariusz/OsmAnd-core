#!/bin/bash

echo "Checking for bash..."
if [ -z "$BASH_VERSION" ]; then
	echo "Invalid shell, re-running using bash..."
	exec bash "$0" "$@"
	exit $?
fi
SRCLOC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source "$SRCLOC/../functions.sh"

configureExternalFromGit "$SRCLOC" "https://github.com/osmandapp/OsmAnd-external-qtbase.git" "qt-v5.3.0-rc1"
cp -rpf "$SRCLOC/upstream.original/mkspecs/winphone-arm-msvc2012" "$SRCLOC/upstream.original/mkspecs/winphone-arm-msvc2013"
cp -rpf "$SRCLOC/upstream.original/mkspecs/winphone-x86-msvc2012" "$SRCLOC/upstream.original/mkspecs/winphone-x86-msvc2013"
patchExternal "$SRCLOC"

# Check if tools are present
if [[ "$(uname -a)" == *Cygwin* ]]; then
	if [ ! -f "$SRCLOC/tools.windows.pack" ]; then
		echo "Downloading Qt build tools for Windows..."
		curl -L https://qt.gitorious.org/qt/qt5/archive-tarball/dev > "$SRCLOC/tools.windows.pack" || { echo "Failure" 1>&2; exit; }
	fi
	if [ ! -d "$SRCLOC/tools.windows" ]; then
		echo "Extracting Qt build tools for Windows..."
		mkdir -p "$SRCLOC/tools.windows"
		tar -xf "$SRCLOC/tools.windows.pack" -C "$SRCLOC/tools.windows" "qt-qt5/gnuwin32" --strip=2
		chmod +x "$SRCLOC/tools.windows/bin"/*
	fi
fi
