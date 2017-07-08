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
    // int pipe_type;
    int bird_direction;

    State(int bird_y, int pipe_type, int bird_direction)
        : bird_y(bird_y) /*, pipe_type(pipe_type)*/, bird_direction(bird_direction) {}

    //State(int index)
    //: bird_y(index % 200), pipe_type(index / 1000), bird_direction((index % 1000) / 200){}

    State(int index)
        : bird_y(index % 300), bird_direction(index / 300) {}

    State(int relative_height, int bird_direction)
        : bird_y(relative_height), bird_direction(bird_direction) {}

    int ToIndex()
    {
        return bird_y + bird_direction * 300;
    }

    void FromIndex(int index)
    {
        bird_y = index % 300;
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