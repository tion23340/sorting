#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <file>"
    exit 1
fi

file="$1"

awk '
NR==1 {
    prev=$1
    next
}
{
    if ($1 < prev) {
        printf("Not sorted at line %d\n", NR)
        exit 1
    }
    prev=$1
}
END {
    print "Sorted"
}
' "$file"
