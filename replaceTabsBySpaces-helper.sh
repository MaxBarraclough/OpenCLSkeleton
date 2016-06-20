sed 's/\t/    /g' < $1 > "$1.notabs"
mv "$1.notabs" $1
