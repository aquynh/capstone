#! /usr/bin/env bash

# Capstone Disassembler Engine
# By Nguyen Anh Quynh <aquynh@gmail.com>, 2013>

# Note: to cross-compile "nix32" on Linux, package gcc-multilib is required.

function build {
	if [ ${MAKE}x = x ]; then
		MAKE=make
	fi

	CROSS= ${MAKE} clean

	if [ ${CC}x != x ]; then
		${MAKE} CC=$CC
	else
		${MAKE}
	fi
}

case "$1" in
  "" ) build;;
  "default" ) build;;
  "nix32" ) CFLAGS=-m32 LDFLAGS=-m32 build;;
  "bsd") MAKE=gmake PREFIX=/usr/local build;;
  "clang" ) CC=clang build;;
  "cross-win32" ) CROSS=i686-w64-mingw32- build;;
  "cross-win64" ) CROSS=x86_64-w64-mingw32- build;;
  "cygwin-mingw32" ) CROSS=i686-pc-mingw32- build;;
  "cygwin-mingw64" ) CROSS=x86_64-w64-mingw32- build;;
  "gcc") CC=gcc build;;
  * ) echo "Usage: compile.sh [nix32|bsd|clang|cross-win32|cross-win64|cygwin-mingw32|cygwin-mingw64|gcc]"; exit 1;;
esac
