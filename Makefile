NEWORLD_SOURCES = $(wildcard source/*.cpp)
NEWORLD_OBJECTS = $(patsubst source/%.cpp, obj/%.o, $(NEWORLD_SOURCES))

INCLUDE_SOURCES = $(wildcard include/AL/*.cpp)
INCLUDE_OBJECTS = $(patsubst include/AL/%.cpp, obj/%.o, $(INCLUDE_SOURCES))

ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -O0
else
    CXXFLAGS += -O2
endif

ifeq ($(NOWARNING), 1)
    CXXFLAGS += -w
else
    CXXFLAGS += -Werror
endif

ifeq ($(shell sh -c 'uname -s'), Darwin)
    CXXFLAGS += -DNEWORLD_TARGET_MACOSX -I /usr/local/include -I /usr/local/include/freetype2
    LDFLAGS += -L /usr/local/lib -framework OpenGL
    ifeq ($(CXX), c++)
	CXX = clang++
    endif
endif

all: NEWorld

NEWorld: obj/ $(NEWORLD_OBJECTS) $(INCLUDE_OBJECTS)
	$(CXX) -o NEWorld $(LDFLAGS) -logg -lvorbis -lvorbisfile -lfreetype -lopenal -lglfw3 $(NEWORLD_OBJECTS) $(INCLUDE_OBJECTS)

obj/:
	mkdir -p obj

obj/%.o: source/%.cpp
	$(CXX) -I include -std=c++11 $(CXXFLAGS) -c $< -o $@ 

obj/%.o: include/AL/%.cpp
	$(CXX) -I include -std=c++11 $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj/*.o NEWorld

