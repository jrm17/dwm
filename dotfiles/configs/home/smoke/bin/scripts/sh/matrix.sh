#!/bin/bash

echo -e "\033[2J\033[?25l"

R=$(tput lines)
C=$(tput cols)
: $[R--]

while true; do
    (
       j=$[RANDOM%C]
       d=$[RANDOM%R]
       for i in $(eval echo -e {1..$R}); do
           c=$(printf '\\\\0%o' $[RANDOM%57+33])
           echo -e "\033[$[i-1];${j}H\033[32m$c\033[$i;${j}H\033[37m"$c
           sleep 0.1
           if [ $i -ge $d ]; then
               echo -e "\033[$[i-d];${j}H "
           fi
       done
       for i in $(eval echo -e {$[i-d]..$R}); do
           echo -e "\033[$i;${j}f "
           sleep 0.1
       done
   ) & sleep 0.05
done
