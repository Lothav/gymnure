SHADER_FILES=phong			\
		 	 path_tracing 	\
		 	 freetype		\
		 	 skybox	        \

all: clear_old 	\
	 build 		\

clear_old:
	$(info Clearing old files:)
	rm -rfv ./*.spv

build:
	$(info Creating SPIRV compiled shaders files:)
	for shader_file in $(SHADER_FILES) ; do 										\
		glslangValidator -e main -o $$shader_file.vert.spv -V -D $$shader_file.vert \
		glslangValidator -e main -o $$shader_file.frag.spv -V -D $$shader_file.frag \
	done
