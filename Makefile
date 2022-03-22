all: dir creator

dir:
	mkdir -p build
	cd build

creator: dir 
	gcc -o build/creator src/creator.c  src/shared_mem.c src/buffer.c src/message.c src/timer.c -lm 
