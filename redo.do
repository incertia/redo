DEPS="build.o util.o redo.o"
./redo-ifchange $DEPS
gcc -o $3 -g -O0 $DEPS
