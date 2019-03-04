#!/usr/bin/env bash


# Small install script for Arch Linux

while true; do
    clear
    echo -e "\nWhat would you like your username to be?
    \n\nDo NOT pick the name of an already existing user. This will overwrite their files!"

    printf "\n\nUsername: "
    read -r USER

    printf "You chose %s for your name. Wanna continue? [y/N]: " "$USER"
    read -r answer

    case $answer in
        y*|Y*) break
    esac
done

useradd -m -g wheel -s /bin/zsh "$USER"
passwd "$USER"
echo "%wheel ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

packages=(
base-devel xorg-xinit xorg-server feh ffmpeg pulseaudio pulseaudio-alsa
pavucontrol pamixer w3m mpv wget rofi python-pip neovim python-neovim
ranger mediainfo poppler htop imagemagick compton gimp rsync scrot unzip unrar
ntfs-3g gpick
)

for pack in "${packages[@]}"; do
    pacman --noconfirm --needed -S "$pack"
done

systemctl enable NetworkManager
systemctl start NetworkManager

echo "Basic System Setup Complete"
