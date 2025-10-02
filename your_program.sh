#!/bin/sh
#
# Use this script to run your program LOCALLY.
#
# Note: Changing this script WILL NOT affect how CodeCrafters runs your program.
#
# Learn more: https://codecrafters.io/program-interface

set -e # Exit early if any commands fail

# Compile the project
(
  cd "$(dirname "$0")" # Ensure compile steps are run within the repository directory
  cmake . && make
)

# Run the shell
exec $(dirname $0)/bin/shell "$@"
