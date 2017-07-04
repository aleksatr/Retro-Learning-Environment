#include <iostream>
#include <vector>
#include "monte_carlo.h"

using namespace std;

namespace object_model
{
    bool MonteCarlo::EvaluateAndImprovePolicy(double reward, bool isFinal)
    {
        bool actionChanged = false;

        for (auto i = 0; i < _history.size(); ++i)
        {
            pair<State, Action> currStateAction = _history[i];
            Action currAction = (Action)currStateAction.second;
            State currState = (State)currStateAction.first;
            reward = reward_history[i];

            int stateIndex = currState.ToIndex();
            int qIndex = currAction * NUMBER_OF_STATES + stateIndex;

            //eval
            q[qIndex] = q[qIndex] + 1.0 / times_visited[qIndex] * (reward - q[qIndex]);
            //improve
            Action previousAction = _actions[stateIndex];
            _actions[stateIndex] = q[stateIndex] > q[stateIndex + NUMBER_OF_STATES] ? NOOP : JUMP;

            actionChanged = actionChanged || previousAction != _actions[stateIndex];
        }

        return actionChanged;
    }
}
