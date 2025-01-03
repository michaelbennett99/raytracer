#!/bin/bash

cd ..

START=$1

if [ -z "$START" ]
then
    START=1
fi

END=$2

if [ -z "$END" ]
then
    END=11
fi


mkdir -p images/adaptive_10000_01

echo "" > images/adaptive_10000_01/timing.txt

for i in $(seq $START $END)
do
    start_time=$(date +%s.%3N)


    ./build/release/raytracer \
        -w 600 \
        -r 1.0 \
        -s 10000 \
        -a \
        -b 256 \
        -c 128 \
        -t 0.1 \
        -d \
        -o images/adaptive_10000_01/scene_$i \
        -f png \
        ${i}

    end_time=$(date +%s.%3N)
    runtime=$(echo "$end_time - $start_time" | bc)

    echo "Scene $i: $runtime seconds" \x
        >> images/adaptive_10000_01/timing.txt
done
