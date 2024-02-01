#!/usr/bin/env bash

build_release() {
	echo "Building Release"
	buildDir="build-release"
	mkdir -p "$buildDir"
	CXX=g++ cmake -G 'Ninja' -S . -B "$buildDir" -DCMAKE_BUILD_TYPE=Release -DENABLE_CLOCK=ON -DENABLE_VISUALIZER=ON && ninja -C "$buildDir"
}

build_debug() {
	echo "Building Debug"
	buildDir="build-debug"
	mkdir -p "build-debug"
	CXX=clang++ cmake -G 'Ninja' -S . -B "$buildDir" -DCMAKE_BUILD_TYPE=Debug -DENABLE_CLOCK=ON -DENABLE_VISUALIZER=ON && ninja -C "$buildDir"
}

MODE="$1"

if [ -z "$MODE" ]; then
	MODE="debug"
fi

case $MODE in
"debug")
	build_debug
	;;
"release")
	build_release
	;;
esac
