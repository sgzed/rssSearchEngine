EXE:= main
INC_DIR:= ./include ./include/cppjieba ./include/simhash ./include/deps
SRC_DIR:= ./src
SRCS:= $(wildcard $(SRC_DIR)/*.cc)
LIBS:= -lboost_regex -lpthread
CXXFLAGS:= -std=c++11 -g

$(EXE):$(SRCS)
	g++ -g -o $@ $^ ./src/tinyxml2.cpp  $(LIBS) $(addprefix -I, $(INC_DIR)) $(CXXFLAGS)
clean:
	rm main

