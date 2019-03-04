#!/bin/bash
# - Position the Cursor:
#   \033[<L>;<C>H
#      Or
#   \033[<L>;<C>f
#   puts the cursor at line L and column C.
# - Move the cursor up N lines:
#   \033[<N>A
# - Move the cursor down N lines:
#   \033[<N>B
# - Move the cursor forward N columns:
#   \033[<N>C
# - Move the cursor backward N columns:
#   \033[<N>D

# - Clear the screen, move to (0,0):
#   \033[2J
# - Erase to end of line:
#   \033[K

# - Save cursor position:
#   \033[s
# - Restore cursor position:
#   \033[u

# PROMPT_COMMAND=prompt_command

clock() {
    local       BLUE="\[\033[0;34m\]"
    local        RED="\[\033[0;31m\]"
    local  LIGHT_RED="\[\033[1;31m\]"
    local      WHITE="\[\033[1;37m\]"
    local  NO_COLOUR="\[\033[0m\]"
    case $TERM in
        xterm*)
            TITLEBAR='\[\033]0;\u@\h:\w\007\]'
            ;;
        *)
            TITLEBAR=""
            ;;
    esac

    PS1="${TITLEBAR}\
\[\033[s\033[1;\$(echo -n \$((COLUMNS - 5)))H\]\
$BLUE[$LIGHT_RED\$(date +%H%M)$BLUE]\[\033[u\033[1A\]
$BLUE[$LIGHT_RED\u@\h:\w$BLUE]\
$WHITE\$$NO_COLOUR "
    PS2='> '
    PS4='+ '
}

clock
