#include "Forward.hpp"
namespace Engine
{
    namespace GraphicsPipeline
    {
        Forward::Forward() : pipeline_ (std::make_unique<Pipeline>()) {}

        uint32_t Forward::createProgram(Programs::ProgramParams &&params)
        {
            programs_.push_back(std::make_shared<Programs::Program>(params, pipeline_->getRenderPass()));
            return static_cast<uint32_t>(programs_.size() - 1);
        }

        void Forward::addObjData(uint32_t program_id, GymnureObjData&& data, const GymnureObjDataType& type)
        {
            if(programs_.size() <= program_id)
                throw "Invalid program ID!";

            programs_[program_id]->addObjData(std::move(data), type);
        }

        void Forward::prepare(const std::shared_ptr<Descriptors::Camera> &camera)
        {
            if(programs_.empty())
                return;

            for (auto& program : programs_)
                program->prepare(camera);

            pipeline_->prepare(programs_);
        }

        void Forward::render()
        {
            if(programs_.empty())
                return;

            pipeline_->render();
        }
    }
}
