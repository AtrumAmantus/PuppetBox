#include "../Pipeline.h"

namespace PB
{

    void TransformPipelineData::addData(const UUID uuid)
    {
        transformVector_->push_back({});
    }

    void TransformPipelineData::removeDataAt(std::uint32_t index)
    {
        transformVector_->erase(transformVector_->begin() + index);
    }

    const std::string TransformPipelineData::getReference() const
    {
        return "pb_transform";
    }

    std::shared_ptr<void> TransformPipelineData::getDataVector() const
    {
        return transformVector_;
    }
}