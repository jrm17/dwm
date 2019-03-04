#!/bin/bash

# exit on first error
set -e

backupdir="/media/wdblue/Backup"

# check if destination dir exists
if [[ ! -d "$backupdir" ]]; then
    echo "Backup directory $backupdir does not exist. Is the drive mounted?"
    exit 1
fi

homedir="$backupdir/home"
START=$(date +%s)

echo "Syncing ~/ to $homedir"
rsync ~/ $homedir -aPhAHX --one-file-system --info=progress2,name0,stats2 --delete

FINISH=$(date +%s)

echo "total time: $(($((FINISH - START)) / 60)) minutes, $(($((FINISH - START)) % 60)) seconds"
