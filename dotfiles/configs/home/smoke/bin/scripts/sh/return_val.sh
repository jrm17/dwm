#!/usr/bin/env bash

n1=${1:-1}
n2=${2:-1}

is_equal() {
    local i1=$1
    local i2=$2
    (( i1 == i2 )) && return 0
    return 1
}

main() {
    if is_equal "$n1" "$n2"; then
        echo "is_equal return code: $?"
        echo "$n1 == $n2"
    else
        echo "is_equal return code: $?"
        echo "$n1 != $n2"
    fi
}

main

exit 0
