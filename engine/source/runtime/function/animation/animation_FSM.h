#pragma once
#include <functional>
#include <json11.hpp>
#include <vector>
namespace Pilot
{
    class AnimationFSM
    {
        // std::vector<AnimationFSMNode> m_nodes;
        enum States
        {
            idle,
            walk_start,
            walk_run,
            walk_stop,
            jump_start_from_idle,
            jump_loop_from_idle,
            jump_end_from_idle,
            jump_start_from_walk_run,
            jump_loop_from_walk_run,
            jump_end_from_walk_run,
        } m_state {idle};

    public:
        AnimationFSM();
        bool   eval(const json11::Json::object& json);
        size_t curIndex();
    };
} // namespace Pilot
