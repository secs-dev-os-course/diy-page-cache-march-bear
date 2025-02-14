#!/bin/bash

set -e

cd "$(dirname "$0")"/..

. ci/commons.bash

MODE=$1
if [ -z "$MODE" ]; then
  MODE="check"
fi

if [ "$MODE" != "fix" ] && [ "$MODE" != "check" ]; then
  error "Invalid argument. Must be either 'fix' or 'check'."
  exit 1
fi

message "Got parameter: '$MODE'"

message "Building the project..."
(cd build && cmake --build .)

if [ "$MODE" = "fix" ]; then
  message "Formatting code..."
  find app -iname '*.h' -o -iname '*.c' \
  | xargs clang-format -i --fallback-style=Google --verbose
fi

message "Checking code format..."
find app -iname '*.h' -o -iname '*.c' \
| xargs clang-format -Werror --dry-run --fallback-style=Google --verbose

message "Checks was passed!"