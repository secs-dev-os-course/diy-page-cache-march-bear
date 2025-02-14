#!/bin/bash

set -e

cd "$(dirname "$0")"/..

. ci/commons.bash

message "Preparing a developement environment..."
mkdir -p build
(cd build && cmake ..)