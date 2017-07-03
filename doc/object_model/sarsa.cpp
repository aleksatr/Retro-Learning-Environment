#include <iostream>
#include <vector>
#include "sarsa.h"

using namespace std;

namespace object_model
{
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
    State currState = (State)currStateAction.first;
    int currStateIndex = currState.ToIndex();
    int currQIndex = currAction * NUMBER_OF_STATES + currStateIndex;
    bool previousAction;

    if (numOfPrevSteps > 1 && !isFinal)
    {
        pair<State, Action> prevStateAction = _history[numOfPrevSteps - 2];
        Action prevAction = (Action)prevStateAction.second;
        State prevState = (State)prevStateAction.first;
        int stateIndex = prevState.ToIndex();
        int prevQIndex = prevAction * NUMBER_OF_STATES + stateIndex;

        //cout << "PrevState y = " << prevState.bird_y << " Type = "
        // << prevState.pipe_type<<" Direction "<<prevState.bird_direction << " Actions: " << _actions[stateIndex]<<endl; //<< q[stateIndex] << ";" << q[stateIndex + NUMBER_OF_STATES] << endl;

        //eval
        q[prevQIndex] = q[prevQIndex] + alpha * ((reward + discount * q[currQIndex]) - q[prevQIndex]);
        //improve
        previousAction = _actions[stateIndex];
        _updated = (previousAction != (_actions[stateIndex] = q[stateIndex] > q[stateIndex + NUMBER_OF_STATES] ? NOOP : JUMP));
    }
    else if (isFinal)
    {
        //eval
        q[currQIndex] = q[currQIndex] + alpha * (reward - q[currQIndex]);
        //improve
        previousAction = _actions[currStateIndex];
        _updated = (previousAction != (_actions[currStateIndex] = q[currStateIndex] > q[currStateIndex + NUMBER_OF_STATES] ? NOOP : JUMP));

        //std::cout << q[currStateIndex] << ";" << q[currStateIndex + NUMBER_OF_STATES] << endl;
        cout << "CurrState y = " << currState.bird_y << " Actions: " << q[currStateIndex] << ";" << q[currStateIndex + NUMBER_OF_STATES] << endl;
    }

    return _updated;
}

Action Sarsa::GetAction(State s)
{
    Action currentAction = Policy::GetAction(s);

    //EvaluateAndImprovePolicy(0.0, false);

    return currentAction;
}

void Sarsa::FlushToDisk(char* filename)
{
    Policy::FlushToDisk(filename);
}
void Sarsa::LoadFromDisk(char* filename)
{
   Policy::LoadFromDisk(filename);
}

}
