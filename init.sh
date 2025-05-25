#!/bin/bash
git submodule update --init --recursive --remote

sed -i "s/ON/OFF/g" "votess-cpu/extern/votess/cmake/options.cmake"
sed -i "s/ON/OFF/g" "votess-gpu/extern/votess/cmake/options.cmake"
