shared_sources = $(wildcard src/shared/*.cpp)
shared_objects = $(patsubst src/shared/%.cpp, obj/shared/%.o, $(shared_sources))

client_sources = $(wildcard src/client/*.cpp)
client_objects = $(patsubst src/client/%.cpp, obj/client/%.o, $(client_sources))

server_sources = $(wildcard src/server/*.cpp)
server_objects = $(patsubst src/server/%.cpp, obj/server/%.o, $(server_sources))

ifeq ($(RELEASE), 1)
    CXXFLAGS += -O2
else
    CXXFLAGS += -g -O0
endif

LDFLAGS = -L ./dependencies/lib

all: client server

client: obj/ obj/client/ obj/shared/ $(client_objects) $(shared_objects)
	$(CXX) -o NEWorld $(LDFLAGS) -logg -lvorbis -lvorbisfile -lfreetype -lglfw3 -lopenal -lopengl32 $(client_objects) $(al_objects) $(shared_objects)

server: obj/ obj/server/ obj/shared/ $(server_objects) $(shared_objects)
	$(CXX) -o NEWorldServer $(server_objects) $(shared_objects)

obj/:
	mkdir -p obj/

obj/client/:
	mkdir -p obj/client/

obj/server/:
	mkdir -p obj/server/

obj/shared/:
	mkdir -p obj/shared/

GENERAL_FLAG = -I ./dependencies/include -std=c++14
C_S_FLAG =  -I ./src/shared

obj/shared/%.o: src/shared/%.cpp
	$(CXX) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

obj/client/%.o: src/client/%.cpp
	$(CXX) $(C_S_FLAG) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

obj/server/%.o: src/server/%.cpp
	$(CXX) $(C_S_FLAG) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj/*.o NEWorld NEWorldServer
