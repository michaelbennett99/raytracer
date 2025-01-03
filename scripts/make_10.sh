#!/bin/bash

cd ..

START=$1
END=$2

if [ -z "$START" ]
then
    START=1
fi

if [ -z "$END" ]
then
    END=11
fi

mkdir -p images/10

echo "" > images/10/timing.txt

for i in $(seq $START $END)
do
    start_time=$(date +%s.%3N)

    ./build/release/raytracer \
        -w 600 \
        -r 1.0 \
        -s 10 \
        -o images/10/scene_$i \
        -f png \
        ${i}

    end_time=$(date +%s.%3N)
    runtime=$(echo "$end_time - $start_time" | bc)

    echo "Scene $i: $runtime seconds" \
        >> images/10/timing.txt
done
