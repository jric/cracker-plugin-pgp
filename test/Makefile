# For C Projects
CC = gcc
CFLAGS = -std=c++17 -pedantic -Wall -g
OBJECTS = test-gpg-plugin.o
LIBS = -lstdc++

# This is the default target
all: test-gpg-plugin

# For C Projects
%.o: %.cc
	$(CC) $(CFLAGS) -c $<

test-gpg-plugin: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

test: ./test-gpg-plugin
	./test-gpg-plugin

clean: <OTHER_CLEAN TARGETS>
	rm -f *.o test-gpg-plugin
	
help:
	@echo "make all | clean | help"
