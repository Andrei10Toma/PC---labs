#!/bin/bash

SPEED=1
DELAY=1
LOSS=0
CORRUPT=0

{
    killall link
    killall recv
    killall send
} &> /dev/null

./link_emulator/link speed=$SPEED delay=$DELAY loss=$LOSS corrupt=$CORRUPT &> /dev/null &
sleep 1
./recv &
sleep 1

./send input_mic.txt test_file.txt test_file_3.txt

#sleep 2
#echo "Finished transfer, checking files"
#diff tema.txt recv_tema.txt
