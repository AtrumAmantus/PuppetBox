#include "../Pipeline.h"

namespace PB
{
    void SingleRenderPipelineData::addData(const UUID uuid)
    {
        const std::uint32_t INDEX = singleRenderVector_->size();

        singleRenderVector_->push_back(SingleRenderData{});
        singleRenderVector_->at(INDEX)
            .transformReference = std::move(std::make_shared<DefaultTransformMatrixReference>(
                uuid,
                entityMap(),
                *singleRenderVector_
            ));
    }

    void SingleRenderPipelineData::removeDataAt(std::uint32_t index)
    {
        singleRenderVector_->erase(singleRenderVector_->begin() + index);
    }

    const std::string SingleRenderPipelineData::getReference() const
    {
        return "pb_single_render";
    }

    std::shared_ptr<void> SingleRenderPipelineData::getDataVector() const
    {
        return singleRenderVector_;
    }
}