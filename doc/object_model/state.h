#ifndef __STATE_HPP__
#define __STATE_HPP__

#include <rle_interface.hpp>

using namespace rle;

namespace object_model
{
class State
{
public:
    int bird_y;
    int pipe_type;
    int bird_direction;

public:
  State(int bird_y, int pipe_type, int bird_direction) 
    : bird_y(bird_y), pipe_type(pipe_type), bird_direction(bird_direction){}
};

enum Action 
{ 
    NOOP = JOYPAD_NOOP,
    JUMP = JOYPAD_A,
    NUMBER_OF_ACTIONS = 2
};
}

#endif // __STATE_HPP__