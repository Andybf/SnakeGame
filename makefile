
sdl_version = USE_SDL=2

result_directory = webRelease
project_name = index
game_lib_name = ~/Desktop/IcosphereGameLibrary/IcosphereGameLibrary/lib/libIcosphereGameLibrary.a
library_include_path = /Users/anderson/Desktop/IcosphereGameLibrary/IcosphereGameLibrary/include

currrent_path = $(shell pwd)

MAKEFLAGS += --silent

all:
	echo "Building..."
	
	emcc -O3\
		-lembind\
		sources/main.cpp\
		$(game_lib_name) -o $(project_name).js\
		-I$(library_include_path)\
		-s USE_WEBGL2=1\
		-s MAX_WEBGL_VERSION=2\
		-s MIN_WEBGL_VERSION=2\
		-s ALLOW_MEMORY_GROWTH=1\
		-s $(sdl_version)\
		--embed-file ./textures/snake-game-sprites.png\
		--embed-file ./textures/fontmap.png\
		--embed-file ./shaders/gameObjectInstanced.fs\
		--embed-file ./shaders/gameObjectInstanced.vs\
		--embed-file ./shaders/gameObject.fs\
		--embed-file ./shaders/gameObject.vs\
		--embed-file ./shaders/text.fs\
		--embed-file ./shaders/text.vs\
	
	if test -d ./$(result_directory); then rm -r ./$(result_directory); fi;
	mkdir $(result_directory);
	cp -r ./webSource/{index.html,main.js,style.css,media} ./$(result_directory);
	mv $(project_name).js $(project_name).wasm ./$(result_directory);
	
	echo "done."
