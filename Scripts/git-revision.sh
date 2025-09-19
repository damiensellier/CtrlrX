#!/bin/bash
ROOT=`git rev-parse --show-toplevel`

# get version from file
V=`cat $ROOT/VERSION`

HASH=`git rev-parse --short HEAD`

echo "$V-$HASH"
