#include "runtime/function/animation/animation_FSM.h"
#include <iostream>
using namespace Pilot;
AnimationFSM::AnimationFSM() {}
float tryGetFloat(const json11::Json::object& json, const std::string& key, float default_value)
{
    auto found_iter = json.find(key);
    if (found_iter != json.end() && found_iter->second.is_number())
    {
        return found_iter->second.number_value();
    }
    return default_value;
}
bool tryGetBool(const json11::Json::object& json, const std::string& key, float default_value)
{
    auto found_iter = json.find(key);
    if (found_iter != json.end() && found_iter->second.is_bool())
    {
        return found_iter->second.bool_value();
    }
    return default_value;
}
bool AnimationFSM::eval(const json11::Json::object& json)
{
    States last_state=m_state;
    bool   is_clip_finish = tryGetBool(json, "clip_finish", false);
    bool   is_jumping  = tryGetBool(json, "jumping", false);
    float  speed          = tryGetFloat(json, "speed", 0);
    bool   is_moving      = speed>0.01f;
    bool   start_walk_end = false;
    //speed > 0.3f && speed < 0.99f;

	switch (m_state)
    {
        case States::idle:
            if (is_jumping)
            {
                m_state = jump_start_from_idle;
            }
            else if (is_moving)
            {
                m_state = walk_start;
            }
            break;
        case States::walk_start:
            if (is_clip_finish)
            {
                m_state = walk_run;
            }
            break;
        case States::walk_run:
            if (is_jumping)
            {
                m_state = jump_start_from_walk_run;
            }
            else if (start_walk_end)
            {
                m_state = walk_stop;
            }
            else if (!is_moving && is_clip_finish)
            {
                m_state = idle;
            }
            break;
        case States::walk_stop:
            if (is_clip_finish)
            {
                m_state = idle;
            }
            break;
        case States::jump_start_from_idle:
            if (is_clip_finish)
            {
                m_state = jump_loop_from_idle;
            }
            break;
        case States::jump_loop_from_idle:
            if (!is_jumping)
            {
                m_state = jump_end_from_idle;
            }
            break;
        case States::jump_end_from_idle:
            if (is_clip_finish)
            {
                m_state = idle;
            }
            break;
        case States::jump_start_from_walk_run:
            if (is_clip_finish)
            {
                m_state = jump_loop_from_walk_run;
            }
            break;
        case States::jump_loop_from_walk_run:
            if (!is_jumping)
            {
                m_state = jump_end_from_walk_run;
            }
            break;
        case States::jump_end_from_walk_run:
            if (is_clip_finish)
            {
                m_state = walk_run;
            }
            break;
        default:
            break;
    }
    if (last_state != m_state)
    {
        std::cout << m_state;
    }
    return last_state != m_state;
}
size_t AnimationFSM::curIndex() {
    switch (m_state)
    {
        case idle:
            return 0;
        case walk_start:
            return 1;
        case walk_run:
            return 0;
        case walk_stop:
            return 2;
        case jump_start_from_walk_run:
        case jump_start_from_idle:
            return 3;
        case jump_loop_from_walk_run:
        case jump_loop_from_idle:
            return 4;
        case jump_end_from_walk_run:
        case jump_end_from_idle:
            return 5;
        default:
            return 0;
    }
}



