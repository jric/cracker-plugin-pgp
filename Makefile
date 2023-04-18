# For C Projects
CC = gcc
CFLAGS = -std=c++17 -pedantic -Wall -g -fPIC `pkg-config --cflags gpgme`
PLUGIN_NAME = crackerplugin_gpg
OBJECTS = plugin.o
LIBS = -lstdc++
LFLAGS = -shared `pkg-config --libs gpgme`
MAJOR_VERSION = 1
VERSION = $(MAJOR_VERSION).0.0

.PHONY: all test clean help

# This is the default target
all: lib$(PLUGIN_NAME).so.$(VERSION)

# For C Projects
%.o: %.cc Makefile
	$(CC) $(CFLAGS) -c $<

lib$(PLUGIN_NAME).so.$(VERSION): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) $(LIBS) -o $@

test: lib$(PLUGIN_NAME).so.$(VERSION)
	$(MAKE) -C test test

clean: <OTHER_CLEAN TARGETS>
	rm -f *.o <BINARY_NAME_HERE> <ETC>

clean-doc:
	rm -rf README.md doc/xml
	
help:
	@echo "make all | test | clean | help"
