CFLAGS += -std=c99
PREFIX ?= /usr

.PHONY: all clean

all: 2048

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -p ./2048 $(DESTDIR)$(PREFIX)/bin/2048

clean:
	rm -f 2048