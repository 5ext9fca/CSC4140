#! /usr/bin/bash
cmake -S . -B build -G ninja
cmake --build build
./build/demo