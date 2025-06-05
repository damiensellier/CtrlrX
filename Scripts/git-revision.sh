#!/bin/bash
projecttag=`git describe --tags --abbrev=0`
revisioncount=`git log --oneline "$projecttag".. | wc -l | tr -d ' '`
echo "$projecttag.$revisioncount"
