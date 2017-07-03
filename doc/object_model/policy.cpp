#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "policy.h"

using namespace std;

namespace object_model
{

Policy::Policy()
{
    for (int i = 0; i < NUMBER_OF_STATES; i++)
    {
        _actions[i] = NOOP;
        q[i] = q[i + NUMBER_OF_STATES] = 0;
    }
    srand(time(NULL));
}

void Policy::ClearHistory()
{
    _history.clear();
}

Policy::~Policy()
{
}

Action Policy::GetAction(State s)
{
    int index = s.ToIndex(); //calculate index from state
    Action a = _actions[index];

    double r = ((double)rand() / (RAND_MAX));

    if (/*Program.Mode == Mode.Train && */ r < epsilon)
    {
        double randAction = ((double)rand() / (RAND_MAX));
        a = randAction > 0.5 ? JUMP : NOOP;
    }

    _history.push_back(s, a);
    return a;
}

void Policy::FlushToDisk()
{
}

}