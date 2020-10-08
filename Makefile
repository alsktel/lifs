# LIFS tool makefile

all: bin_dir 
	@gcc -I./include -m32 -w -o bin/mklifs ./src/main.c \
	./src/bitmap.c ./src/header.c src/commons.c src/file.c src/directory.c

bin_dir:
	@if [ ! -d ./bin ]; then mkdir ./bin; fi

install:
	@cp ./bin/mklifs /bin/mklifs
	@chmod u+x /bin/mklifs

unistall:
	@rm /bin/mklifs
