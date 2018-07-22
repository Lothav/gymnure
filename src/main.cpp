#include <memory>
#include "../gymnure.h"

int main(int argc, char** argv) {

	const unsigned windowWidth  = 800;
	const unsigned windowHeight = 600;

	{
		auto gymnure = std::make_unique<Gymnure>(windowWidth, windowHeight);

		auto vert = Engine::GraphicPipeline::Shader{};
		vert.type = VK_SHADER_STAGE_VERTEX_BIT;
		vert.path = "../../shaders/vert.spv";

		auto frag = Engine::GraphicPipeline::Shader{};
		frag.type = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag.path = "../../shaders/frag.spv";

		auto room2 = GymnureData{};
		room2.path_texture 	= "../../assets/room2.png";
		room2.path_obj 		= "../../assets/room2.obj";
		room2.shaders 		= {vert, frag};

		gymnure->insertData(room2);

		while(true) {
			if (!gymnure->draw()) {
				break;
			}
		}
	}

	return EXIT_SUCCESS;
}
