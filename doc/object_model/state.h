#ifndef __STATE_HPP__
#define __STATE_HPP__

#include <rle_interface.hpp>

using namespace rle;

namespace object_model
{
class State
{
  public:
    int height_difference;
    int bird_direction;

    State(int height_difference, int pipe_type, int bird_direction)
        : height_difference(height_difference), bird_direction(bird_direction) {}

    State(int index)
        : height_difference(index % 300), bird_direction(index / 300) {}

    State(int relative_height, int bird_direction)
        : height_difference(relative_height), bird_direction(bird_direction) {}

    int ToIndex()
    {
        return height_difference + bird_direction * 300;
    }

    void FromIndex(int index)
    {
        height_difference = index % 300;
        //pipe_type = index / 1000;
        bird_direction = index / 300;
    }
};

enum Action
{
    NOOP = JOYPAD_NOOP,
    JUMP = JOYPAD_B,
    NUMBER_OF_ACTIONS = 2
};
}

#endif // __STATE_HPP__