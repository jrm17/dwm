#!/usr/bin/env bash
 
${ZSH_VERSION+false} || emulate ksh
${BASH_VERSION+shopt -s lastpipe extglob}

# colorSet [ --setaf | --setab | --misc ] var
# Assigns the selected set of escape mappings to the given associative array names.
function colorSet {
    typeset -a clrs msc
    typeset x
    clrs=(black red green orange blue magenta cyan grey darkgrey ltred ltgreen yellow ltblue ltmagenta ltcyan white)
    msc=(sgr0 bold dim smul blink rev invis)

    while ! ${2:+false}; do
        ${KSH_VERSION:+eval typeset -n "$2"=\$2}
        case ${1#--} in
            setaf|setab)
                for x in "${!clrs[@]}"; do
                    eval "$2"'[${clrs[x]}]=$(tput "${1#--}" "$x")'
                done
                ;;
            misc)
                for x in "${msc[@]}"; do
                    eval "$2"'[$x]=$(tput "$x")'
                done
                ;;
            *)
                return 1
        esac
        shift 2
    done
}

# Example code
function main {
    typeset -A fgColors bgColors miscEscapes
    if colorSet --setaf fgColors --setab bgColors --misc miscEscapes; then
        if ! ${1:+${fgColors[$1]:+false}}; then
            printf '%s%s%s\n' "${fgColors[$1]}" "this text is ${1}" "${miscEscapes[sgr0]}" >&3
        else
            printf '%s, %s\n' "${1:-Empty}" 'no such color.'
            typeset x y
            for x in fgColors bgColors miscEscapes; do
                typeset -a keys
                eval 'keys=("${!'"$x"'[@]}")'
                printf '%s=( ' "$x"
                for y in "${keys[@]}"; do
                    eval 'printf "[%q]=%q " "$y" "${'"$x"'[$y]}"'
                done
                printf ')\n'
            done
            return 1
        fi
    else
        echo 'Failed setting color arrays.'
        return 1
    fi 3>&1 >&2
}

main "$@"

# vim: set fenc=utf-8 ff=unix ft=sh :
