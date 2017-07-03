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
        timesVisited[i] = timesVisited[i + NUMBER_OF_STATES] = 0;
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

Action Policy::GetAction(int bird_pos, int pipe_type, int bird_direction)
{
    //int ace = hasUsableAce ? 1 : 0;
    //int index = ace * 100 + (dealerCard % 10) * 10 + currentSum % 10;

    int index = 0; //calculate index from state
    Action a = _actions[index];

    double r = ((double)rand() / (RAND_MAX));

    if (/*Program.Mode == Mode.Train && */ r < epsilon)
    {
        double randAction = ((double)rand() / (RAND_MAX));
        a = randAction > 0.5 ? JUMP : NOOP;
    }

    timesVisited[(int)a * NUMBER_OF_STATES + index]++;

    // _history.push_back(hasUsableAce * 100 + (dealerCard % 10) * 10 + currentSum % 10, a);
    return a;
}

///returns true if best action for any state is changed
// virtual bool Policy::EvaluateAndImprovePolicy(double reward, bool isFinal = true) = 0;

void Policy::FlushToDisk()
{
    // string text = string.Join(Environment.NewLine, timesVisited);

    // text += Environment.NewLine + string.Join(Environment.NewLine, q);
    // File.WriteAllText(Program.FileName, text);
}
}