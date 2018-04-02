MUDUO_DIRECTORY ?= $(HOME)/build/release-cpp11
#MUDUO_DIRECTORY ?= $(HOME)/build/install
MUDUO_INCLUDE = $(HOME)/muduo ./include ./include/cppjieba ./include/simhash ./include/deps
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
SRCDIR:= ./src
SOURCE = $(wildcard $(SRCDIR)/*.cc)

CXXFLAGS = -g -std=c++11 -Wall -Wextra -Werror \
		   -Wno-unused-parameter  \
		   -march=native -rdynamic \

LDFLAGS = -L$(MUDUO_LIBRARY) -L/usr/local/lib -lmuduo_net -lmuduo_base -lpthread -lhiredis -lrt

all: searchServer
clean:
	rm -f searchServer core

searchServer: $(SOURCE)
	g++ $(CXXFLAGS) -o $@ $^  $(addprefix -I, $(MUDUO_INCLUDE))  $(LDFLAGS) 

.PHONY: all clean

