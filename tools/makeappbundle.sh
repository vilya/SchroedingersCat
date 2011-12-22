#/bin/bash

if [ $# -lt 3 ]; then
  echo "Usage: $0 <target-app-file> <executable> <resource-dir>"
  exit 1
fi

target=$1
executable=$2
resourcedir=$3

targetexe=$target/Contents/MacOS/`basename $executable`
echo "=== targetexe = $targetexe"

if [ ! -x $executable ]; then
  if [ -e $executable ]; then
    echo "Executable file $executable is not flagged as executable."
  else
    echo "Executable file $executable does not exist."
  fi
  exit 1
fi

if [ ! -e $resourcedir ]; then
  echo "Resource dir $resourcedir doesn't exist."
  exit 1
fi

if [ `basename $target` == `basename -s .app $target` ]; then
  echo "Target $target has the wrong suffix, must end in .app"
  exit 1
fi

mkdir -p $target
mkdir -p $target/Contents
mkdir -p $target/Contents/MacOS
mkdir -p $target/Contents/Resources
cp tools/Info.plist $target/Contents
cp $executable $target/Contents/MacOS
cp tools/rungame.sh $target/Contents/MacOS
cp -R $resourcedir/* $target/Contents/Resources

cp /opt/local/lib/libjpeg.8.dylib $target/Contents/MacOS
cp /opt/local/lib/libtiff.3.dylib $target/Contents/MacOS
cp /opt/local/lib/libpng14.14.dylib $target/Contents/MacOS
cp /opt/local/lib/libz.1.dylib $target/Contents/MacOS

install_name_tool -change /opt/local/lib/libjpeg.8.dylib @executable_path/libjpeg.8.dylib $targetexe
install_name_tool -change /opt/local/lib/libtiff.3.dylib @executable_path/libtiff.3.dylib $targetexe
install_name_tool -change /opt/local/lib/libpng14.14.dylib @executable_path/libpng14.14.dylib $targetexe
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/libz.1.dylib $targetexe

