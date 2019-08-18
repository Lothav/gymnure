#include "Deferred.hpp"

namespace Engine
{
    namespace GraphicsPipeline
    {
        Deferred::Deferred() : pipeline_ (std::make_unique<Pipeline>((uint32_t)1)) {}

        uint32_t Deferred::createProgram(Programs::ProgramParams &&mrt, Programs::ProgramParams &&present)
        {
            vk::RenderPass renderPass = pipeline_->getRenderPass();

            Passes passes;
            passes.mrt = std::make_shared<Programs::Program>(std::move(mrt), renderPass);
            passes.present = std::make_shared<Programs::Program>(std::move(present), renderPass);

            programs_.push_back(passes);

            return static_cast<uint32_t>(programs_.size() - 1);
        }

        void Deferred::addObjData(uint32_t program_id, GymnureObjData&& data)
		{
            if(programs_.size() <= program_id)
                throw std::exception("Invalid program ID!");

            // Add object only to MRT pass.
            programs_[program_id].mrt->addObjData(std::move(data));
            object_count_++;
        }

        void Deferred::prepare(const std::shared_ptr<Descriptors::Camera> &camera)
        {
            if(programs_.empty() || object_count_ == 0)
                return;

            std::vector<std::shared_ptr<Programs::Program>> programs = {};
            for(auto& program : programs_) {
                program.mrt->prepare(camera);
                program.present->prepare(camera);

                programs.push_back(program.mrt);
                programs.push_back(program.present);
            }

            pipeline_->prepare(programs);
        }

        void Deferred::render()
        {
            if(programs_.empty() || object_count_ == 0)
                return;

            pipeline_->render();
        }
    }
}
