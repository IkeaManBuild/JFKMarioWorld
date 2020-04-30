odir="$PWD"
cd ..
if [ "$1" == save ]; then
    (g++ -O3 -std=c++2a -o jfkmarioworld -fpermissive JFKMarioWorld.cpp -lSDL2 -lSDL2_mixer -lSDL2_image -llua5.3 -I/usr/include/SDL2 -I/usr/include/lua5.3 -I/usr/include/SFML) > outfile 2>&1
else
    g++ -O3 -std=c++2a -o jfkmarioworld -fpermissive JFKMarioWorld.cpp -lSDL2 -lSDL2_mixer -lSDL2_image -llua5.3 -I/usr/include/SDL2 -I/usr/include/lua5.3 -I/usr/include/SFML
fi
cd "$odir"