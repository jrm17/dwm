#!/usr/bin/env bash


E_RUNERR=65

# Check if md5sum and bc are installed.
if ! which bc &> /dev/null; then
    echo bc is not installed.
    echo "Cant run . . . "
    exit $E_RUNERR
fi
if ! which md5sum &> /dev/null; then
    echo md5sum is not installed.
    echo "Cant run . . . "
    exit $E_RUNERR
fi

USLEEP_ARG=0

trap 'echo -en "\E[?25h"; echo -en "\E[0m"; stty echo; tput cup 20 0; rm -fr  $HORSE_RACE_TMP_DIR'  TERM EXIT

# Set a unique (paranoid) name for the temp directory the script needs.
HORSE_RACE_TMP_DIR=$HOME/.horserace-$(date +%s)-$(head -c10 /dev/urandom | md5sum | head -c30)

# Create the temp directory and move right in.
mkdir "$HORSE_RACE_TMP_DIR"
cd "$HORSE_RACE_TMP_DIR" || exit


move_and_echo() {
    echo -ne "\E[${1};${2}H""$3"
}

# Function to generate a pseudo-random number between 1 and 9.
random_1_9 () {
    head -c10 /dev/urandom | md5sum | tr -d [a-z] | tr -d 0 | cut -c1
}

#  Two functions that simulate "movement," when drawing the horses.
draw_horse_one() {
    echo -n " "//$MOVE_HORSE//
}
draw_horse_two(){
    echo -n " "\\\\$MOVE_HORSE\\\\
}


# Define current terminal dimension.
N_COLS=$(tput cols)
N_LINES=$(tput lines)

# Need at least a 20-LINES X 80-COLUMNS terminal. Check it.
if [ "$N_COLS" -lt 80 ] || [ "$N_LINES" -lt 20 ]; then
    echo "$(basename "$0") needs a 80-cols X 20-lines terminal."
    echo "Your terminal is ${N_COLS}-cols X ${N_LINES}-lines."
    exit $E_RUNERR
fi

# Need a string of 80 chars. See below.
BLANK80=$(seq -s "" 100 | head -c80)

clear
echo -ne '\E[37;47m'
tput cup 0 0

for n in $(seq 5); do
    echo "$BLANK80"
done
echo -ne '\E[30m'

move_and_echo 3 1 "START  1"
move_and_echo 3 75 FINISH
move_and_echo 1 5 "|"
move_and_echo 1 80 "|"
move_and_echo 2 5 "|"
move_and_echo 2 80 "|"
move_and_echo 4 5 "|  2"
move_and_echo 4 80 "|"
move_and_echo 5 5 "V  3"
move_and_echo 5 80 "V"

# Set foreground color to red.
echo -ne '\E[31m'

# Some ASCII art.
move_and_echo 1 8 "..@@@..@@@@@...@@@@@.@...@..@@@@..."
move_and_echo 2 8 ".@...@...@.......@...@...@.@......."
move_and_echo 3 8 ".@@@@@...@.......@...@@@@@.@@@@...."
move_and_echo 4 8 ".@...@...@.......@...@...@.@......."
move_and_echo 5 8 ".@...@...@.......@...@...@..@@@@..."
move_and_echo 1 43 "@@@@...@@@...@@@@..@@@@..@@@@."
move_and_echo 2 43 "@...@.@...@.@.....@.....@....."
move_and_echo 3 43 "@@@@..@@@@@.@.....@@@@...@@@.."
move_and_echo 4 43 "@..@..@...@.@.....@.........@."
move_and_echo 5 43 "@...@.@...@..@@@@..@@@@.@@@@.."


# Set foreground and background colors to green.
echo -ne '\E[32;42m'

# Draw  eleven green lines.
tput cup 5 0
for n in $(seq 11); do
    echo "$BLANK80"
done

# Set foreground color to black.
echo -ne '\E[30m'
tput cup 5 0

# Draw the fences.
echo "++++++++++++++++++++++++++++++++++++++\
++++++++++++++++++++++++++++++++++++++++++"

tput cup 15 0
echo "++++++++++++++++++++++++++++++++++++++\
++++++++++++++++++++++++++++++++++++++++++"

# Set foreground and background colors to white.
echo -ne '\E[37;47m'

# Draw three white lines.
for n in $(seq 3); do
    echo "$BLANK80"
done

# Set foreground color to black.
echo -ne '\E[30m'

# Create 9 files to stores handicaps.
for n in $(seq 10 7 68); do
    touch "$n"
done

# Set the first type of "horse" the script will draw.
HORSE_TYPE=2

#  Create position-file and odds-file for every "horse".
#+ In these files, store the current position of the horse,
#+ the type and the odds.
for HN in $(seq 9); do
    touch horse_${HN}_position
    touch odds_${HN}
    echo \-1 > horse_${HN}_position
    echo $HORSE_TYPE >>  horse_${HN}_position
    # Define a random handicap for horse.
    HANDICAP=$(random_1_9)
    # Check if the random_1_9 function returned a good value.
    while ! echo $HANDICAP | grep [1-9] &> /dev/null; do
        HANDICAP=$(random_1_9)
    done
    # Define last handicap position for horse.
    LHP=$(expr "$HANDICAP" \* 7 + 3)
    for FILE in $(seq 10 7 $LHP); do
        echo "$HN" >> "$FILE"
    done

    # Calculate odds.
    case $HANDICAP in
        1) ODDS=$(echo "$HANDICAP" \* 0.25 + 1.25 | bc)
            echo "$ODDS" > "odds_${HN}"
            ;;
        2 | 3) ODDS=$(echo "$HANDICAP" \* 0.40 + 1.25 | bc)
            echo "$ODDS" > "odds_${HN}"
            ;;
        4 | 5 | 6) ODDS=$(echo "$HANDICAP" \* 0.55 + 1.25 | bc)
            echo "$ODDS" > "odds_${HN}"
            ;;
        7 | 8) ODDS=$(echo "$HANDICAP" \* 0.75 + 1.25 | bc)
            echo "$ODDS" > "odds_${HN}"
            ;;
        9) ODDS=$(echo "$HANDICAP" \* 0.90 + 1.25 | bc)
            echo "$ODDS" > "odds_${HN}"
    esac


done


# Print odds.
print_odds() {
    tput cup 6 0
    echo -ne '\E[30;42m'
    for HN in $(seq 9); do
        echo "#$HN odds->" $(cat odds_${HN})
    done
}

# Draw the horses at starting line.
draw_horses() {
    tput cup 6 0
    echo -ne '\E[30;42m'
    for HN in $(seq 9); do
        echo /\\$HN/\\"                               "
    done
}
# "
print_odds

echo -ne '\E[47m'
tput cup 17 0
echo -e '\E[?25l'Press [enter] key to start the race...
read -s
stty -echo

# --------------------------------------------------------
# Start the race.

draw_horses
echo -ne '\E[37;47m'
move_and_echo 18 1 "$BLANK80"
echo -ne '\E[30m'
move_and_echo 18 1 Starting...
sleep 1

WINNING_POS=74
START_TIME=$(date +%s)
COL=0

while [ $COL -lt $WINNING_POS ]; do

    MOVE_HORSE=0

    # Check if the random_1_9 function has returned a good value.
    while ! echo "$MOVE_HORSE" | grep '[1-9]' &> /dev/null; do
        MOVE_HORSE=$(random_1_9)
    done

    # Define old type and position of the "randomized horse".
    HORSE_TYPE=$(cat horse_${MOVE_HORSE}_position | tail -n 1)
    COL=$((cat "horse_${MOVE_HORSE}_position" | head -n 1))

    ADD_POS=1
    # Check if the current position is an handicap position.
    if seq 10 7 68 | grep -w $COL &> /dev/null; then
        if grep -w $MOVE_HORSE $COL &> /dev/null; then
            ADD_POS=0
            grep -v -w  $MOVE_HORSE $COL > ${COL}_new
            rm -f $COL
            mv -f ${COL}_new $COL
        else ADD_POS=1
        fi
    else ADD_POS=1
    fi
    COL=$((COL + ADD_POS))
    echo "$COL" >  "horse_${MOVE_HORSE}_position"  # Store new position.

    # Choose the type of horse to draw.
    case $HORSE_TYPE in
        1) HORSE_TYPE=2; DRAW_HORSE=draw_horse_two
            ;;
        2) HORSE_TYPE=1; DRAW_HORSE=draw_horse_one
    esac
    echo "$HORSE_TYPE" >>  "horse_${MOVE_HORSE}_position"
    # Store current type.

    # Set foreground color to black and background to green.
    echo -ne '\E[30;42m'

    # Move the cursor to new horse position.
    tput cup `expr $MOVE_HORSE + 5` \
        `cat  horse_${MOVE_HORSE}_position | head -n 1`

    # Draw the horse.
    $DRAW_HORSE
    usleep $USLEEP_ARG

    # When all horses have gone beyond field line 15, reprint odds.
    touch fieldline15
    if [ $COL = 15 ]; then
        echo $MOVE_HORSE >> fieldline15
    fi
    if [ `wc -l fieldline15 | cut -f1 -d " "` = 9 ]; then
        print_odds
        : > fieldline15
    fi

    # Define the leading horse.
    HIGHEST_POS=`cat *position | sort -n | tail -1`
    echo -ne '\E[47m'
    tput cup 17 0
    echo -n Current leader: `grep -w $HIGHEST_POS *position | cut -c7`\
        "                              "
done

FINISH_TIME=`date +%s`

echo -ne '\E[30;42m'
echo -en '\E[5m'

tput cup `expr $MOVE_HORSE + 5` \
    `cat  horse_${MOVE_HORSE}_position | head -n 1`
$DRAW_HORSE

echo -en '\E[25m'
echo -ne '\E[37;47m'
move_and_echo 18 1 $BLANK80
echo -ne '\E[30m'
tput cup 17 0
echo -e "\E[5mWINNER: $MOVE_HORSE\E[25m""  Odds: `cat odds_${MOVE_HORSE}`"\
"  Race time: `expr $FINISH_TIME - $START_TIME` secs"
echo -en "\E[?25h"
echo -en "\E[0m"

stty echo
rm -rf $HORSE_RACE_TMP_DIR
tput cup 19 0

exit 0
