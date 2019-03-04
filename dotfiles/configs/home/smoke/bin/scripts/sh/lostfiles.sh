#!/bin/bash


set -eo pipefail

if [ $UID != "0" ]; then
    echo "You must run this script as root." 1>&2
    exit 1
fi

usage() {
    echo " lostfile @VERSION@"
    echo " Usage: $0 [-s] [-z] [-h]"
    echo "      Calling without an option runs in relaxed mode without sorting"
    echo "  -h  display this help"
    echo "  -s  optionally define strict mode"
    echo "  -z  optionally sort results by size"
    exit 0
}

# setup defaults which user can override with switches
use_filter=filter
# 0 = relaxed (default)
# 1 = strict
MODE=0

while getopts 'sz' OPTION; do
    case "$OPTION" in
        z)
            use_filter=filter_sort
            ;;
        s)
            MODE=1
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND -1))

filter() {
    # just filter
    sed -e 's|^\t||;' | sort
}

filter_sort() {
    sed -e '1d' | tr '\n' '\0' | xargs -0 -n1 du -s | sort -rn -k1
}

strict() {
    export LC_ALL=C
    comm -13 \
        <(pacman -Qlq | sed -e 's|/$||' | sort -u) \
        <(find /boot /etc /opt /srv /usr /var | sort -u)
}

# relaxed mode is more forgiving about hits

relaxed() {
    export LC_ALL=C
    # get current kernel version for excluding current dkms module folder in /usr/lib/modules
    CURRENTKERNEL="$(uname -r)"

    comm -13 \
        <(pacman -Qlq | sed -e 's|/$||' | sort -u) \
        <(find /boot /etc /opt /usr /var -not \( \
        -wholename '/boot/syslinux' -prune -o \
        -wholename '/boot/grub' -prune -o \
        -wholename '/boot/loader' -prune -o \
        -wholename '/boot/EFI' -prune -o \
        -wholename '/boot/initramfs*' -prune -o \
        -wholename '/boot/lost+found' -prune -o \
        -wholename '/etc/.etckeeper' -prune -o \
        -wholename '/etc/.git' -prune -o \
        -wholename '/etc/.pwd.lock' -prune -o \
        -wholename '/etc/.updated' -prune -o \
        -wholename '/etc/blkid.tab' -prune -o \
        -wholename '/etc/ca-certificates' -prune -o \
        -wholename '/etc/conf.d' -prune -o \
        -wholename '/etc/dfs' -prune -o \
        -wholename '/etc/pacman.d/gnupg' -prune -o \
        -wholename '/etc/adjtime' -prune -o \
        -wholename '/etc/dhcpcd.duid' -prune -o \
        -wholename '/etc/digitalocean' -prune -o \
        -wholename '/etc/fancontrol' -prune -o \
        -wholename '/etc/gshadow' -prune -o \
        -wholename '/etc/gshadow-' -prune -o \
        -wholename '/etc/gshadow.OLD' -prune -o \
        -wholename '/etc/easy-rsa' -prune -o \
        -wholename '/etc/openvpn' -prune -o \
        -wholename '/etc/group' -prune -o \
        -wholename '/etc/group-' -prune -o \
        -wholename '/etc/group.OLD' -prune -o \
        -wholename '/etc/hostname' -prune -o \
        -wholename '/etc/letsencrypt' -prune -o \
        -wholename '/etc/locale.conf' -prune -o \
        -wholename '/etc/localtime' -prune -o \
        -wholename '/etc/ld.so.cache' -prune -o \
        -wholename '/etc/machine-id' -prune -o \
        -wholename '/etc/network.d/ethernet-static' -prune -o \
        -wholename '/etc/os-release' -prune -o \
        -wholename '/etc/passwd' -prune -o \
        -wholename '/etc/passwd-' -prune -o \
        -wholename '/etc/passwd.OLD' -prune -o \
        -wholename '/etc/shadow' -prune -o \
        -wholename '/etc/shadow-' -prune -o \
        -wholename '/etc/shadow.OLD' -prune -o \
        -wholename '/etc/udev/hwdb.bin' -prune -o \
        -wholename '/etc/ssh/ssh_host*' -prune -o \
        -wholename '/etc/NetworkManager/system-connections' -prune -o \
        -wholename '/etc/rc.digitalocean' -prune -o \
        -wholename '/etc/samba/private/passdb.tdb' -prune -o \
        -wholename '/etc/samba/private/secrets.tdb' -prune -o \
        -wholename '/etc/samba/private/smbpasswd' -prune -o \
        -wholename '/etc/samba/smb.conf' -prune -o \
        -wholename '/etc/ssl' -prune -o \
        -wholename '/etc/xml/catalog' -prune -o \
        -wholename '/etc/systemd/system' -prune -o \
        -wholename '/etc/systemd/network' -prune -o \
        -wholename '/etc/udev/rules.d/70-digitalocean-net.rules' -prune -o \
        -wholename '/etc/vconsole.conf' -prune -o \
        -wholename '/etc/zfs/zpool.cache' -prune -o \
        -wholename '/opt/google-appengine-go/appengine/api/*.pyc' -prune -o \
        -wholename '/usr/bin/__pycache__' -prune -o \
        -wholename '/usr/lib/virtualbox/ExtensionPacks/Oracle_VM_VirtualBox_Extension_Pack' -prune -o \
        -wholename '/usr/lib/locale/locale-archive' -prune -o \
        -wholename '/usr/lib/modules/'$CURRENTKERNEL'' -prune -o \
        \( -wholename '/usr/lib/node_modules*' -and -not -wholename '/usr/lib/node_modules/npm/*' \) -o \
        -wholename '/usr/local' -prune -o \
        -wholename '/usr/share/dict/words' -prune -o \
        -wholename '/usr/share/info/dir' -prune -o \
        -wholename '/usr/share/.mono/certs/Trust' -prune -o \
        -wholename '/usr/share/applications/mimeinfo.cache' -prune -o \
        -wholename '/usr/share/backintime' -prune -o \
        -wholename '/usr/share/fonts/TTF/fonts.dir' -prune -o \
        -wholename '/usr/share/fonts/TTF/fonts.scale' -prune -o \
        -wholename '/usr/share/fonts/misc/fonts.dir' -prune -o \
        -wholename '/usr/share/fonts/misc/fonts.scale' -prune -o \
        -wholename '/usr/share/mime' -prune -o \
        -wholename '/usr/share/webapps/owncloud/data' -prune -o \
        -wholename '/usr/var/cache' -prune -o \
        -wholename '/usr/var/run' -prune -o \
        -wholename '/var/.updated' -prune -o \
        -wholename '/var/lost+found' -prune -o \
        -wholename '/var/abs' -prune -o \
        -wholename '/var/cache' -prune -o \
        -wholename '/var/lock' -prune -o \
        -wholename '/var/lib' -prune -o \
        -wholename '/var/log' -prune -o \
        -wholename '/var/run' -prune -o \
        -wholename '/var/spool' -prune -o \
        -wholename '/var/db/sudo' -prune -o \
        -wholename '/var/tmp' -prune \) | sort -u \
        )
}


if [[ $MODE -eq 0 ]]; then
    relaxed | $use_filter
else
    strict | $use_filter
fi
