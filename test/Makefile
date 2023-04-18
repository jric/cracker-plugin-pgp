ifndef BOOST_ROOT
$(error make sure to install boost and set environment variable, BOOST_ROOT, e.g. /usr/local/Cellar/boost)
endif

# For C Projects
CC = gcc
CFLAGS = -std=c++17 -pedantic -Wall -g -I$(BOOST_ROOT)/include
OBJECTS = test-gpg-plugin.o
LIBS = -lstdc++ -L$(BOOST_ROOT)/lib -lboost_system -lboost_filesystem
PHONY_TARGETS = all:test:clean:help

.PHONY: $(subst :, ,$(PHONY_TARGETS))

# This is the default target
all: test-gpg-plugin

# For C Projects
%.o: %.cc
	$(CC) $(CFLAGS) -c $<

test-gpg-plugin: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@

test: test-gpg-plugin
	./test-gpg-plugin

clean: <OTHER_CLEAN TARGETS>
	rm -f *.o test-gpg-plugin
	
help:
	@echo $(subst :," | ",$(PHONY_TARGETS))
