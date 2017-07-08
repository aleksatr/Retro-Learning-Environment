#include <iostream>
#include <vector>
#include "sarsa.h"

#define TD_N 2

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
    Action previousAction;

    if (numOfPrevSteps > TD_N && !isFinal)
    {
        double gama = 1;
        int currentIndex = currQIndex;
        for (int i = 0; i < TD_N; i++)
        {
            gama *= discount;
            int index = numOfPrevSteps - 2 - i;

            currentIndex = EvalueTD(reward, index, currentIndex, gama);
        }
    }
    else if (isFinal)
    {
        //eval
        q[currQIndex] = q[currQIndex] + alpha * (reward - q[currQIndex]);
        //improve
        previousAction = _actions[currStateIndex];
        _updated = (previousAction != (_actions[currStateIndex] = q[currStateIndex] > q[currStateIndex + NUMBER_OF_STATES] ? NOOP : JUMP));
    }

    return _updated;
}

int Sarsa::EvalueTD(double reward, int index, int currentIndex, double gama)
{

    pair<State, Action> prevStateAction = _history[index];
    Action prevAction = (Action)prevStateAction.second;
    State prevState = (State)prevStateAction.first;
    int stateIndex = prevState.ToIndex();
    int prevQIndex = prevAction * NUMBER_OF_STATES + stateIndex;
    
    //eval
    q[prevQIndex] = q[prevQIndex] + alpha * ((reward + gama * q[currentIndex]) - q[prevQIndex]);

    //improve
    Action previousAction = _actions[stateIndex];
    _updated = (previousAction != (_actions[stateIndex] = q[stateIndex] > q[stateIndex + NUMBER_OF_STATES] ? NOOP : JUMP));

    return prevQIndex;
}

Action Sarsa::GetAction(State s)
{
    Action currentAction = Policy::GetAction(s);

    //EvaluateAndImprovePolicy(0.0, false);

    return currentAction;
}
}
