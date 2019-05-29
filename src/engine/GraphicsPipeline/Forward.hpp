#ifndef GYMNURE_FORWARD_HPP
#define GYMNURE_FORWARD_HPP

#include <GraphicsPipeline/Pipeline.hpp>

namespace Engine
{
	namespace GraphicsPipeline
	{
		class Forward
		{

		private:

			std::unique_ptr<Pipeline>                           forward_pipeline_ = {};
            std::vector<std::shared_ptr<Programs::Program>>     programs_ = {};

		public:

			Forward();

			uint32_t createProgram(Programs::ProgramParams &&params);
			void addObjData(uint program_id, GymnureObjData&& data);
			void prepare(const std::shared_ptr<Descriptors::Camera> &camera);
			void render();
		};
	}
}

#endif //GYMNURE_FORWARD_HPP
