#pragma once

#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/resource/res_type/data/blend_state.h"

#include <string>
#include <vector>

namespace Pilot
{

    REFLECTION_TYPE(AnimationResultElement)
    CLASS(AnimationResultElement, WhiteListFields)
    {
        REFLECTION_BODY(AnimationResultElement);

    public:
        int        index;
        Matrix4x4_ transform;
    };

    REFLECTION_TYPE(AnimationResult)
    CLASS(AnimationResult, Fields)
    {
        REFLECTION_BODY(AnimationResult);

    public:
        std::vector<AnimationResultElement> node;
    };

    REFLECTION_TYPE(AnimationComponentRes)
    CLASS(AnimationComponentRes, WhiteListFields)
    {
        REFLECTION_BODY(AnimationComponentRes);

    public:

        META(Enable) std::string                           skeleton_file_path;
        META(Enable) std::vector<Reflection::ReflectionPtr<BlendState>> blend_states;
        // animation to skeleton map
        float frame_position; // 0-1
        AnimationResult animation_result;
        ~AnimationComponentRes() { 
            for (auto& blend_state : blend_states)
            {
                PILOT_REFLECTION_DELETE(blend_state);
            }
        }
        static void deepCopy(AnimationComponentRes & dst, const AnimationComponentRes& src)
        {
            dst = src;
            for (int i = 0; i < src.blend_states.size(); i++)
            {
                if (src.blend_states[i].getTypeName() == "BlendState")
                {
                    dst.blend_states[i].getPtrReference() = new BlendState();
                    *static_cast<BlendState*>(dst.blend_states[i]) =
                        *static_cast<const BlendState*>(src.blend_states[i]);
                }
                else if (src.blend_states[i].getTypeName() == "BlendState1D")
                {
                    dst.blend_states[i].getPtrReference() = new BlendState1D();
                    *static_cast<BlendState1D*>(dst.blend_states[i]) =
                        *static_cast<const BlendState1D*>(src.blend_states[i]);
                }
            }
        }
    };

    REFLECTION_TYPE(AnimationComponents)
    CLASS(AnimationComponents, Fields)
    {
        REFLECTION_BODY(AnimationComponents);

    public:
        std::string                        schemaFile;
        std::vector<AnimationComponentRes> components;
    };

} // namespace Pilot