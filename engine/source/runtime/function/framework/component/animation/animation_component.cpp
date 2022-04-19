#include "runtime/function/framework/component/animation/animation_component.h"

#include "runtime/engine.h"
#include "runtime/function/animation/animation_system.h"
#include "runtime/function/framework/object/object.h"

namespace Pilot
{
    AnimationComponent::AnimationComponent(const AnimationComponentRes& animation_component_ast,
                                           GObject*                     parent_object) :
        Component(parent_object)
    {
        AnimationComponentRes::deepCopy(animation_component, animation_component_ast);
        auto skeleton_res = AnimationManager::tryLoadSkeleton(animation_component.skeleton_file_path);

        skeleton.buildSkeleton(*skeleton_res);
    }

    void AnimationComponent::blend1D(float desired_ratio, BlendState1D* blend_state)
    {
        if (blend_state->values.size() < 2)
        {
            // no need to interpolate
            return;
        }
        const auto& key_it    = signal.find(blend_state->key);
        double      key_value = 0;
        if (key_it != signal.end())
        {
            if (key_it->second.is_number())
            {
                key_value = key_it->second.number_value();
            }
        }
        int max_smaller = -1;
        for (auto value : blend_state->values)
        {
            if (value <= key_value)
            {
                max_smaller++;
            }
            else
            {
                break;
            }
        }

        for (auto& weight : blend_state->blend_weight)
        {
            weight = 0;
        }
        if (max_smaller == -1)
        {
            blend_state->blend_weight[0] = 1.0f;
        }
        else if (max_smaller == blend_state->values.size() - 1)
        {
            blend_state->blend_weight[max_smaller] = 1.0f;
        }
        else
        {
            auto l = blend_state->values[max_smaller];
            auto r = blend_state->values[max_smaller + 1];

            if (l == r)
            {
                // some error
            }

            float weight = (key_value - l) / (r - l);

            blend_state->blend_weight[max_smaller + 1] = weight;
            blend_state->blend_weight[max_smaller]     = 1 - weight;
        }
        blend(desired_ratio);
    }
    void AnimationComponent::tick(float delta_time)
    {
        if ((m_tick_in_editor_mode == false) && g_is_editor_mode)
            return;
        size_t blend_state_index = animation_fsm.curIndex();
        if (blend_state_index >= animation_component.blend_states.size() || blend_state_index < 0)
        {
            return;
        }
        float length = animation_component.blend_states[blend_state_index]->getBlendedLength();
        float delta_ratio   = delta_time / length;
        float desired_ratio = delta_ratio + ratio;
        if (desired_ratio >= 1.f)
        {
            desired_ratio = desired_ratio - floor(desired_ratio);
            updateSignal("clip_finish", true);
        }
        else 
        {
            updateSignal("clip_finish", false);
        }
        bool  restart       = animation_fsm.eval(signal);
        if (!restart)
        {
            ratio = desired_ratio;
        }
        else
        {
            ratio = 0;
        }

        blend_state_index = animation_fsm.curIndex();
        if (blend_state_index >= animation_component.blend_states.size() || blend_state_index < 0)
        {
            return;
        }

        if (animation_component.blend_states[blend_state_index].getTypeName() == "BlendState1D")
        {
            auto blend_state_1d_pre = static_cast<BlendState1D*>(animation_component.blend_states[blend_state_index]);
            blend1D(ratio, blend_state_1d_pre);
        }
        else if (animation_component.blend_states[blend_state_index].getTypeName() == "BlendState")
        {
            blend(ratio);
        }
    }

    const AnimationResult& AnimationComponent::getResult() const { return animation_component.animation_result; }

    void AnimationComponent::blend(float desired_ratio)
    {
        size_t blend_state_index = animation_fsm.curIndex();
        for (auto& ratio : animation_component.blend_states[blend_state_index]->blend_ratio)
        {
            ratio = desired_ratio;
        }
        auto blendStateData =
            AnimationManager::getBlendStateWithClipData(*animation_component.blend_states[blend_state_index]);
        std::vector<AnimationPose> poses;
        for (int i = 0; i < blendStateData.clip_count; i++)
        {
            AnimationPose pose(blendStateData.blend_clip[i],
                               blendStateData.blend_weight[i],
                               blendStateData.blend_ratio[i],
                               blendStateData.blend_anim_skel_map[i]);
            skeleton.resetSkeleton();
            skeleton.applyAdditivePose(pose);
            skeleton.extractPose(pose);
            poses.push_back(pose);
        }
        for (int i = 1; i < blendStateData.clip_count; i++)
        {
            for (auto& pose : poses[i].m_weight.blend_weight)
            {
                pose = animation_component.blend_states[blend_state_index]->blend_weight[i];
            }
            poses[0].blend(poses[i]);
        }


        skeleton.applyPose(poses[0]);
        animation_component.animation_result = skeleton.outputAnimationResult();
    }

} // namespace Pilot
