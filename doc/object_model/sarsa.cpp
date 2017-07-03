#include <iostream>
#include <vector>
#include "sarsa.h"

using namespace std;

namespace object_model
{
// class Sarsa : Policy
// {
    void Sarsa::ClearHistory()
    {
        Policy::ClearHistory();
        _updated = false;
    }

    bool Sarsa::EvaluateAndImprovePolicy(double reward, bool isFinal)
    {
        int numOfPrevSteps = _history.size();
        pair<State, Action> currStateAction = _history[numOfPrevSteps - 1];
        Action currAction = (Action)currStateAction.second;
        State currActionIndex = (State)currStateAction.first;
       // int currQIndex = currAction * NUMBER_OF_STATES + currActionIndex;
        bool previousAction;

        if (numOfPrevSteps > 1 && !isFinal)
        {
            pair<State, Action> prevStateAction = _history[numOfPrevSteps - 2];
            Action prevAction = (Action)prevStateAction.second;
            State prevActionIndex = (State)prevStateAction.first;
            //int prevQIndex = prevAction * NUMBER_OF_STATES + prevActionIndex;
//
            ////eval
            //q[prevQIndex] = q[prevQIndex] + alpha * ((reward + discount * q[currQIndex]) - q[prevQIndex]);
            ////improve
            //previousAction = _actions[prevActionIndex];
            //_updated = _updated || (previousAction != (_actions[prevActionIndex] = q[prevActionIndex] > q[prevActionIndex + NUMBER_OF_STATES] ? false : true));
        }
        else if (isFinal)
        {
            //eval
         //   q[currQIndex] = q[currQIndex] + alpha * (reward - q[currQIndex]);
            //improve
         //   previousAction = _actions[currActionIndex];
         //   _updated = _updated || (previousAction != (_actions[currActionIndex] = q[currActionIndex] > q[currActionIndex + NUMBER_OF_STATES] ? false : true));
        }

        return _updated;
    }

    Action Sarsa::GetAction(int bird_pos, int pipe_type, int bird_direction)
    {
        Action currentAction = Policy::GetAction(bird_pos, pipe_type, bird_direction);

        EvaluateAndImprovePolicy(0.0, false);

        return currentAction;
    }
// };
}
