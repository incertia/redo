DEPS="obj/build.o obj/util.o obj/redo.o"
./bin/redo-ifchange $DEPS
gcc -o $3 -g -O0 $DEPS
