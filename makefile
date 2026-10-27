TARGETS = range up_tles up_list json2tle

all: $(TARGETS)

range: range.c
	$(CC) -Wall -Wextra -pedantic -Werror -O3 -o range range.c -I ~/include ~/lunar/liblunar.a -lm -lz

up_tles: up_tles.c
	$(CC) -Wall -Wextra -pedantic -Werror -O3 -o up_tles up_tles.c

up_list: up_list.c
	$(CC) -Wall -Wextra -pedantic -Werror -O3 -o up_list up_list.c

json2tle: json2tle.c
	$(CC) -Wall -Wextra -pedantic -Werror -O3 -o json2tle json2tle.c

clean:
	rm $(TARGETS)
