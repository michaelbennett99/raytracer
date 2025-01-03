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

mkdir -p images/adaptive_100

echo "" > images/adaptive_100/timing.txt

for i in $(seq $START $END)
do
    start_time=$(date +%s.%3N)

    ./build/release/raytracer \
        -w 600 \
        -r 1.0 \
        -s 100 \
        -a \
        -b 16 \
        -c 16 \
        -t 0.05 \
        -d \
        -o images/adaptive_100/scene_$i \
        -f png \
        ${i}

    end_time=$(date +%s.%3N)
    runtime=$(echo "$end_time - $start_time" | bc)

    echo "Scene $i: $runtime seconds" \
        >> images/adaptive_100/timing.txt
done
