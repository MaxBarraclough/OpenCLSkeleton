#!/bin/bash

## http://stackoverflow.com/a/246128
DIR=$( cd "$( dirname "$0" )" && pwd )

cd $DIR

ls $DIR | grep "\.cpp\|\.hpp\|\.rl\|.cl" | xargs -n 1 -P6 "$DIR/replaceTabsBySpaces-helper.sh"

## we don't bother worrying about *directories* with file-like names like *(.rl|.hpp|.cpp)
## nor do we worry about silliness like myfile.cpp.xml

