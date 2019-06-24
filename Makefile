LIB := liboddl

CFLAGS ?= -std=c89 -pedantic -march=native -Wall -g
CFLAGS += -I.

PREFIX ?= $(HOME)/.local
INCLUDEDIR ?= include
LIBDIR ?= lib

LIB_OBJECTS := $(patsubst %.c,%.o,$(wildcard src/*.c))
LIB_OBJECTS += $(patsubst %.l,%.o,$(wildcard src/*.l))

TESTS := $(patsubst %.c,%,$(wildcard tests/*.c))

.PHONY: all clean install
all: $(TESTS)

$(LIB).a: $(LIB_OBJECTS)
	$(AR) rcs $@ $^

tests/%: tests/%.o $(LIB).a
	$(CC) -o $@ $^ $(LDFLAGS)

liboddl.pc:
	printf 'prefix=%s\nincludedir=%s\nlibdir=%s\n\nName: %s\nDescription: %s\nVersion: %s\nCflags: -I$${includedir}\nLibs: -L$${libdir} -l$(LIB)\n' \
		'$(PREFIX)' '$${prefix}/$(INCLUDEDIR)' '$${prefix}/$(LIBDIR)' '$(LIB)' 'Parser for the Open Data Description Language' '0' > $@

clean:
	rm $(TESTS)

D := $(if $(DESTDIR),$(DESTDIR)/)$(PREFIX)
install: $(LIB).a liboddl.pc
	mkdir -p $(D)/$(INCLUDEDIR) $(D)/$(LIBDIR)/pkgconfig
	cp -R liboddl $(D)/$(INCLUDEDIR)
	cp $(LIB).a $(D)/$(LIBDIR)
	cp liboddl.pc $(D)/$(LIBDIR)/pkgconfig
