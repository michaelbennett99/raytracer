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

mkdir -p images/1000

touch images/1000/timing.txt

for i in $(seq $START $END)
do
    start_time=$(date +%s.%3N)

    time ./build/release/raytracer \
        -w 600 \
        -r 1.0 \
        -s 1000 \
        -o images/1000/scene_$i \
        -f png \
        ${i}

    end_time=$(date +%s.%3N)
    runtime=$(echo "$end_time - $start_time" | bc)

    echo "Scene $i: $runtime seconds" \
        >> images/1000/timing.txt
done
