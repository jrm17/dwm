#!/usr/bin/env bash

for i in {1..100}; do
    ret=""
    ((i % 3 == 0)) && ret="Fizz"
    ((i % 5 == 0)) && ret="${ret}Buzz"
    [[ -n $ret ]]  && echo "$ret" || echo "$i"
done
