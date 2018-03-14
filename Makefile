project:  directories fresh_disk other_disk build

directories:
	mkdir -p bin

fresh_disk:
	cp refs/diskimage bin/diskimage

other_disk:
	cp refs/diskimage bin/otherdisk

build:
	gcc -m32 -g -w -std=c11 \
		*c \
		functions/*.c \
		functions/utilities/* \
		functions/level-1/*.c \
		functions/level-2/*.c \
		functions/level-3/*.c \
		-o bin/runfs