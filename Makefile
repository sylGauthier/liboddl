LIB := liboddl.a

CFLAGS ?= -std=c89 -pedantic -march=native -Wall -O3 -fPIC
CFLAGS += -I.
LDFLAGS :=

LIB_OBJECTS := $(patsubst %.c,%.o,$(wildcard src/*.c))
LIB_OBJECTS += $(patsubst %.l,%.o,$(wildcard src/*.l))

TESTS := $(patsubst %.c,%,$(wildcard tests/*.c))

.PHONY: all
all: $(TESTS)

$(LIB): $(LIB_OBJECTS)
	$(AR) rcs $@ $^

tests/%: tests/%.o $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TESTS) $(LIB) $(LIB_OBJECTS)
