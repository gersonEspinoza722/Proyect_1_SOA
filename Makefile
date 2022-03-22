all: dir creator

dir:
	mkdir -p build
	cd build

creator: dir 
	gcc -o build/creator src/creator.c  src/shared_mem.c src/buffer.c src/message.c src/timer.c -lm 
	
	gcc -o build/producer src/producer.c src/shared_mem.c src/buffer.c src/timer.c -lm
	
	gcc -o build/finisher src/finisher.c src/shared_mem.c src/buffer.c src/timer.c -lm
