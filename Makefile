OBJ := $(patsubst %.c,%.o,$(wildcard src/*.c src/*/*.c))
OBJ += $(patsubst %.l,%.o,$(wildcard src/*.l src/*/*.l))
CFLAGS ?= -std=c89 -pedantic -march=native -Wall -g
LDFLAGS :=

TESTS := $(patsubst %.c,%,$(wildcard tests/*.c))

.PHONY: all
all: $(TESTS)

tests/%: tests/%.o $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm $(TESTS)
