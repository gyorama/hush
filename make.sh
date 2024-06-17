if [ ! -d "build" ]; then
    mkdir build
fi

gcc -O3 -Wall -Wextra -FPIC -c src/hush.c -o build/hush.o
ar rcs build/libhush.a build/hush.o

gcc -O3 -Lbuild -Wall -Wextra src/main.c -o build/hush -lhush