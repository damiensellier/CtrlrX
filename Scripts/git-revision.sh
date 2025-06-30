#!/bin/bash
projecttag=`git describe --tags --abbrev=0`
if [ -z "$projecttag" ]; then
    projecttag="v0.0.1"
    git rev-parse --short HEAD
else
    revisioncount=`git log --oneline "$projecttag".. | wc -l | tr -d ' '`
    echo "$projecttag.$revisioncount"
fi