#include "OpenGLModel.h"

namespace PB
{
	OpenGLModel::OpenGLModel(std::vector<RenderedMesh> renderedMeshes) : renderedMeshes_(renderedMeshes)
	{

	}

	void OpenGLModel::render(mat3 transform) const
	{
        for (auto& rMesh_ : renderedMeshes_)
        {
            rMesh_.render(transform);
        }
	}
}