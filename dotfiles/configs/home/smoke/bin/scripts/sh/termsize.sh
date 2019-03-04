#!/bin/bash
# Dynamically display terminal window size


t_size() {
    clear
    LINES=$(tput lines)
    COLUMNS=$(tput cols)
    echo "Width = $COLUMNS Height = $LINES"
}

t_size # call once just to start, the rest is automated


trap t_size WINCH # terminal size change

while true; do
    sleep 0.1
done
