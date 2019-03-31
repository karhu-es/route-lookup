SRC = my_route_lookup.c io.c io.h utils.c utils.h
CFLAGS = -Wall -O3

all: my_route_lookup

my_route_lookup: clean $(SRC)
	gcc $(CFLAGS) $(SRC) -o my_route_lookup -lm

.PHONY: clean

clean:
	rm -f my_route_lookup

run:
	./my_route_lookup ../linearSearch/routing_table.txt ../linearSearch/prueba0.txt
