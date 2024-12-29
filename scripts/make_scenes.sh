cd ..

ARG=$1

if [ -z "$ARG" ]; then
    echo "Usage: $0 <output_dir>"
    exit 1
else
    mkdir -p images/$ARG
fi

for i in {1..10}
do
    ./build/release/raytracer $i > images/$ARG/scene_$i.ppm
done
