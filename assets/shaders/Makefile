VERT_SOURCES = $(wildcard *.vert)
FRAG_SOURCES = $(wildcard *.frag)

build_vert:
	for shader_file in $(VERT_SOURCES) ; do								\
		./glslangValidator -o $$shader_file.vert.spv -V $$shader_file; 	\
	done

build_frag:
	for shader_file in $(FRAG_SOURCES) ; do								\
		./glslangValidator -o $$shader_file.vert.spv -V $$shader_file; 	\
	done

clear_old:
	$(info [Compiling SPIR-V Shaders] Clearing old files:)
	rm -rfv ./*.spv

build:
	$(info [Compiling SPIR-V Shaders] Compiling sources:) \
	build_vert \
	build_frag

all: clear_old 	\
	 build

