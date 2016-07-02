shared_sources = $(wildcard src/shared/*.cpp)
shared_objects = $(patsubst src/shared/%.cpp, obj/shared/%.o, $(shared_sources))

client_sources = $(wildcard src/client/*.cpp)
client_objects = $(patsubst src/client/%.cpp, obj/client/%.o, $(client_sources))

server_sources = $(wildcard src/server/*.cpp)
server_objects = $(patsubst src/server/%.cpp, obj/server/%.o, $(server_sources))

precomp_sources = $(wildcard src/precomp/*.cpp)
precomp_objects = $(patsubst src/precomp/%.cpp, obj/precomp/%.o, $(precomp_sources))

launcher_sources = $(wildcard src/launcher/*.cpp)
launcher_objects = $(patsubst src/launcher/%.cpp, obj/launcher/%.o, $(launcher_sources))

ifeq ($(RELEASE), 1)
    CXXFLAGS += -O2
else
    CXXFLAGS += -g -O0
endif

LDFLAGS = -L ./dependencies/lib -L ./dependencies/lib/boostlib

all: client server launcher

client: obj/ obj/client/ shared $(client_objects) Bin/
	$(CXX) -o Bin/NEWorld.so $(LDFLAGS) -lfreetype $(client_objects) $(shared_objects) $(precomp_objects) -lpthread -ldl

server: obj/ obj/server/ shared $(server_objects) Bin/
	$(CXX) -o Bin/NEWorldServer.so $(server_objects) $(shared_objects) $(precomp_objects) -lpthread -ldl

launcher: obj/launcher/ $(launcher_objects)
	$(CXX) -o NEWorldLauncher $(launcher_objects) $(precomp_objects) -ldl

shared: obj/shared/ obj/precomp/ $(shared_objects) $(precomp_objects)

obj/:
	mkdir -p obj/

obj/client/:
	mkdir -p obj/client/

obj/server/:
	mkdir -p obj/server/

obj/shared/:
	mkdir -p obj/shared/

obj/precomp/:
	mkdir -p obj/precomp/

obj/launcher/:
	mkdir -p obj/launcher/

Bin/:
	mkdir -p Bin/

GENERAL_FLAG = -I ./dependencies/include -std=c++14
C_S_FLAG =  -I ./src/shared

obj/shared/%.o: src/shared/%.cpp
	$(CXX) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

obj/client/%.o: src/client/%.cpp
	$(CXX) $(C_S_FLAG) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

obj/server/%.o: src/server/%.cpp
	$(CXX) $(C_S_FLAG) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

obj/precomp/%.o: src/precomp/%.cpp
	$(CXX) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

obj/launcher/%.o: src/launcher/%.cpp
	$(CXX) $(C_S_FLAG) $(GENERAL_FLAG) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj NEWorld NEWorldServer
