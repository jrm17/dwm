#!/bin/bash

export LC_ALL=zh_CN.UTF-8   # if LC_ALL not work, you could try also "LANG" and "LANGUAGE"
export TEXTDOMAINDIR=/usr/share/locale

# export TEXTDOMAIN="<textdomain>"   # <- optional, set this to save the "<textdomain>" argument for `gettext` below
LANG_HELLO_WORLD="$( gettext "<textdomain>" "Your message to translate" )"

echo $LANG_HELLO_WORLD
