ProgName=Micropolis-SDL2
FilesExcluded=src/WinMenu.cpp src/WinMenu.h

Sources:=$(filter-out $(FilesExcluded), $(wildcard src/*.cpp))
Headers:=$(filter-out $(FilesExcluded), $(wildcard src/*.h))
Objs:=$(patsubst src/%.cpp, objs/%.o, ${Sources})

LDLIBS= -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#CXXFLAGS:= -g

${ProgName}: ${Objs}
	$(CXX)  -o $@ $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS)

objs/%.o: src/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

${Sources}: ${Headers}

clean:
	rm ${Objs} ${ProgName}

.PHONY: clean

