#!/bin/bash

# 16 colors
for clbg in {40..47} {100..107} 49; do #Background
    for clfg in {30..37} {90..97} 39; do #Foreground
        for attr in 0 1 2 4 5 7; do #Formatting
            echo -en "\e[${attr};${clbg};${clfg}m ^[${attr};${clbg};${clfg}m \e[0m"
        done
        echo #Newline
    done
done

# 256 colors
for fgbg in 38 48 ; do # Foreground / Background
    for color in {0..255} ; do # Colors
        printf "\e[${fgbg};5;%sm  %3s  \e[0m" $color $color
        if [[ $((($color + 1) % 6)) -eq 4 ]]; then # Display 6 colors per lines
            echo # New line
        fi
    done
    echo # New line
done
