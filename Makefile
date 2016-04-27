NEWORLD_SOURCES = $(wildcard source/*.cpp)
NEWORLD_OBJECTS = $(patsubst source/%.cpp, obj/%.o, $(NEWORLD_SOURCES))

INCLUDE_SOURCES = $(wildcard include/AL/*.cpp)
INCLUDE_OBJECTS = $(patsubst include/AL/%.cpp, obj/%.o, $(INCLUDE_SOURCES))

PLUGINSDK_SOURCES = $(wildcard PluginSDK/src/*.cpp)
PLUGINSDK_OBJECTS = $(patsubst PluginSDK/src/%.cpp, obj/%.o, $(PLUGINSDK_SOURCES))

ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -O0
else
    CXXFLAGS += -O2
endif

ifeq ($(PUBLICBUILD), 1)
    CXXFLAGS += -DPUBLIC_BUILD
endif

ifeq ($(NOWARNING), 1)
    CXXFLAGS += -w
else
    CXXFLAGS += -Werror
endif

ifeq ($(shell sh -c 'uname -s'), Darwin)
    CXXFLAGS += -DNEWORLD_TARGET_MACOSX -I /usr/local/include -I /usr/local/include/freetype2
    LDFLAGS += -L /usr/local/lib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    ifeq ($(CXX), c++)
	CXX = clang++
    endif
endif

all: NEWorld

NEWorld: obj/ $(NEWORLD_OBJECTS) $(INCLUDE_OBJECTS) $(PLUGINSDK_OBJECTS)
	$(CXX) -o NEWorld $(LDFLAGS) -logg -lvorbis -lvorbisfile -lfreetype -lopenal -lglfw3 $(NEWORLD_OBJECTS) $(INCLUDE_OBJECTS) $(PLUGINSDK_OBJECTS)

obj/:
	mkdir -p obj

obj/%.o: source/%.cpp
	$(CXX) -I include -std=c++11 $(CXXFLAGS) -c $< -o $@ 

obj/%.o: include/AL/%.cpp
	$(CXX) -I include -std=c++11 $(CXXFLAGS) -c $< -o $@

obj/%.o: PluginSDK/src/%.cpp
	$(CXX) -fPIC -std=c++11 $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj/*.o NEWorld

