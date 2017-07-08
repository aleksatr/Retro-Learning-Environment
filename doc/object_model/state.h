#ifndef __STATE_HPP__
#define __STATE_HPP__

#include <rle_interface.hpp>

using namespace rle;

namespace object_model
{
class State
{
public:
    int bird_direction; //od -200 do 199
    int height_difference; //recimo da ih ima 6, od 0 do 5
    
  State(int height_difference, int bird_direction) 
    : height_difference(height_difference), bird_direction(bird_direction){}
    
    State(int index) 
    : height_difference(index % 400 - 200), bird_direction(index / 400){}

    int ToIndex()
    {
        return bird_direction * 400 + 200 + height_difference;
    }

    void FromIndex(int index)
    {
        bird_direction = index / 200;
        height_difference = index % 400 - 200;
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