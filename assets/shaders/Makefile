SHADER_FILES=default        \
             #phong			\
		 	 #path_tracing 	\
		 	 #freetype		\
		 	 #skybox        \

all: clear_old 	\
	 build

clear_old:
	$(info Clearing old files:)
	rm -rfv ./*.spv

build:
	$(info Creating Source Parser file:)
	for shader_file in $(SHADER_FILES) ; do													\
    	./glslangValidator -o $$shader_file.vert.spv -V $$shader_file.vert ; 	\
    	./glslangValidator -o $$shader_file.frag.spv -V $$shader_file.frag ; 	\
	done
