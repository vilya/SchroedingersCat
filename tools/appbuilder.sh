#!/bin/bash

if [ $# -lt 4 ]; then
  echo "Usage: $0 <app-file> <executable> <plist> <resource-dir>"
  exit 1
fi

app_path=$1
app_name=`basename $app_path`
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

echo mkdir -p $app_name
echo pushd $app_name
echo mkdir -p Contents/MacOS
echo mkdir -p Contents/Frameworks
echo mkdir -p Contents/Resources
echo cp $executable Contents/MacOS
echo cp -R $resources/\* Contents/Resources
echo cp $plist Contents/Info.plist
echo popd
echo chmod +x $app_name

