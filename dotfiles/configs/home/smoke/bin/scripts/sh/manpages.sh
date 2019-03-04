#!/bin/bash

term=st
page="$(find /usr/share/man -name '*.gz' | sed 's|.*/\(.*\)\.[0-9]\.gz|\1|g' | sort | uniq -u | dmenu "$@")"
(( ${#page} )) && $term -e man "$page"
