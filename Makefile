CXX = g++
CXXFLAGS = -g -Wall -Iinclude -std=c++11
VPATH = src
LDLIBS = -lWs2_32
RM = del /Q /F
TARGET = Cperver
PAGES = MyPage.dll
_OBJS = main.o Server.o Request.o Response.o
OBJS = $(patsubst %,obj/%,$(_OBJS))

$(TARGET): $(OBJS) $(PAGES)
	$(CXX) $(CXXFLAGS)   -o $@ $^ $(LDLIBS)
	
%.dll: page/%.cpp
	$(CXX) $(CXXFLAGS)   -shared -o $@ $^

obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS)   -c -o $@ $<

.PHONY:
clean:
	$(RM) obj\\*.o *.exe *.dll
