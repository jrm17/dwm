#!/bin/bash

get_BT() {
    if [[ -d "/sys/firmware/efi/" ]]; then
        SYS="UEFI"
    else
        SYS="BIOS"
    fi

    readonly BT="$SYS (x86_64)"
}

get_BT

echo "$BT"

BT="NEW"  # cannot be changed due to: declare -gr

echo "$BT"
