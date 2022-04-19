#pragma once

#include "runtime/function/animation/skeleton.h"
#include "runtime/function/animation/pose.h"
#include "runtime/function/framework/component/component.h"
#include "runtime/resource/res_type/components/animation.h"
#include <runtime/function/animation/animation_FSM.h>
#include "json11.hpp"
namespace Pilot
{
    REFLECTION_TYPE(AnimationComponent)
    CLASS(AnimationComponent : public Component, WhiteListFields)
    {
        REFLECTION_BODY(AnimationComponent)
        Skeleton skeleton;
        META(Enable)
        AnimationComponentRes animation_component;
        AnimationFSM          animation_fsm;
        json11::Json::object  signal;
        float                 ratio {0};
    public:
        AnimationComponent() = default;
        AnimationComponent(const AnimationComponentRes& animation_component_ast, GObject* parent_object);

        void                   tick(float delta_time) override;
        void                   destroy() override {}
        const AnimationResult& getResult() const;
        void                   blend(float desired_ratio);
        void                   blend1D(float desired_ratio, BlendState1D* blend_state);
        template<typename T>
        void updateSignal(const std::string& key, const T& value)
        {
            signal[key] = value;
        }
    };
} // namespace Pilot
