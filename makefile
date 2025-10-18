all: range up_tles

range: range.c
	$(CC) -Wall -Wextra -pedantic -o range range.c -I ~/include ~/lunar/liblunar.a -lm -lz

up_tles: up_tles.c
	$(CC) -Wall -Wextra -pedantic -Werror -o up_tles up_tles.c

clean:
	rm range up_tles
