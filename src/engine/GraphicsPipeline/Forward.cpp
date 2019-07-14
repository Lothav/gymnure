#include "Forward.hpp"
namespace Engine
{
    namespace  GraphicsPipeline
    {
        Forward::Forward() : forward_pipeline_ (std::make_unique<Pipeline>()) {}

        uint32_t Forward::createProgram(Programs::ProgramParams &&params)
        {
            programs_.push_back(std::make_shared<Programs::Program>(params, forward_pipeline_->getRenderPass()));
            return static_cast<uint32_t>(programs_.size() - 1);
        }

        void Forward::addObjData(uint32_t program_id, GymnureObjData&& data)
        {
            if(programs_.size() <= program_id)
                throw "Invalid program ID!";

            programs_[program_id]->addObjData(std::move(data));
        }

        void Forward::prepare(const std::shared_ptr<Descriptors::Camera> &camera)
        {
            for (auto& program : programs_)
                program->prepare(camera);

            forward_pipeline_->prepare(programs_);
        }

        void Forward::render()
        {
            forward_pipeline_->render();
        }
    }
}
