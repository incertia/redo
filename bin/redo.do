DEPS="obj/build.o obj/util.o obj/redo.o"
redo-ifchange $DEPS
gcc -o $3 -g -O0 $DEPS
