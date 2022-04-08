#pragma once

#include <string>

namespace Constant
{
    namespace Asset
    {
        namespace Animation
        {
            const std::string kHumanWalk = "Assets1/Animation/Walk";
            const std::string kIdle0 = "Assets1/Animation/Idle_0";
        }

        namespace Skeleton
        {
            const std::string kHuman = "Assets1/Skeleton/BasicHuman";
        }

        namespace Texture
        {
            const std::string kHumanHead = "Assets1/Texture/TestHead";
            const std::string kHumanBody = "Assets1/Texture/TestBody";
            const std::string kHumanHand = "Assets1/Texture/TestHand";
        }
    }

    namespace DefaultAsset
    {
        namespace Mesh
        {
            const std::string kSprite = "Default/Mesh/Sprite";
        }

        namespace Shader
        {
            const std::string kBasicShader = "Default/Shader/Basic";
            const std::string kUIGlyphShader = "Default/Shader/UI/Glyph";
        }
    }

    namespace Reference
    {
        namespace Skeleton
        {
            const std::string kRoot = "root";
            const std::string kHead = "head";
            const std::string kLeftHand = "left_hand";
            const std::string kRightHand = "right_hand";
        }
    }
}