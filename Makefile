CXX = g++
CXXFLAGS = -g -Wextra -Iinclude -std=c++11
VPATH = src
LDLIBS = -lWs2_32
RM = del /Q /F
TARGET = Cperver
_OBJS = main.o Server.o Request.o Response.o
OBJS = $(patsubst %,obj/%,$(_OBJS))

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS)   -o $@ $^ $(LDLIBS)
	
obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS)   -c -o $@ $<

.PHONY:
clean:
	$(RM) obj\\*.o *.exe
