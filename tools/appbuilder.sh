#!/bin/bash

if [ $# -lt 4 ]; then
  echo "Usage: $0 <app-file> <executable> <plist> <resource-dir>"
  exit 1
fi


app_path=$1
executable=$2
plist=$3
resources=$4


if [ ! -x $executable ]; then
  if [ -e $executable ]; then
    echo "Exeutable file $executable isn't flagged as executable."
  else
    echo "Executable file $executable doesn't exist."
  fi
  exit 1
fi

if [ ! -e $plist ]; then
  echo "Plist file $plist doesn't exist."
  exit 1
fi

if [ ! -x $resources ]; then
  echo "Resource dir $resources doesn't exist."
  exit 1
fi

mkdir -p $app_path
mkdir -p $app_path/Contents/MacOS
mkdir -p $app_path/Contents/Frameworks
mkdir -p $app_path/Contents/Resources
cp $executable $app_path/Contents/MacOS
cp -R $resources/* $app_path/Contents/Resources
cp $plist $app_path/Contents/Info.plist
chmod +x $app_path
# TODO: find all the necessary dylibs, copy them into the Frameworks folder and
# adjust the link paths in the executable to match.

