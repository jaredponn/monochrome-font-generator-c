#!/usr/bin/env bash
set -eu pipefail

# DESCRIPTION
# ~~~~~~~~~~~
# Runs all tests in the `test-cases` directory

find "./test-cases" -name '*.sh' -print0 \
  | xargs -0 -P0 -I{} bash -c '
    TEST_WORKING_DIRECTORY=$(mktemp -d)
    trap "rm -rf \"$TEST_WORKING_DIRECTORY\"" EXIT
    cd "$TEST_WORKING_DIRECTORY"
    bash "$1"
  ' _ {}
