#include <iostream>
#include <vector>
#include "sarsa.h"

using namespace std;

namespace object_model
{
class Sarsa : Policy
{
  private:
    bool _updated;

  public:
    void ClearHistory()
    {
        base::ClearHistory();
        _updated = false;
    }

    bool EvaluateAndImprovePolicy(double reward, bool isFinal = true)
    {
        int numOfPrevSteps = _history.size();
        pair<int, bool> currStateAction = _history[numOfPrevSteps - 1];
        int currAction = (int)currStateAction.second;
        int currAce = currStateAction.Key.HasUsableAce ? 1 : 0;
        int currActionIndex = currStateAction.first;
        int currQIndex = currAction * 200 + currActionIndex;
        bool previousAction;

        if (numOfPrevSteps > 1 && !isFinal)
        {
            pair<int, bool> prevStateAction = _history[numOfPrevSteps - 2];
            int prevAction = (int)prevStateAction.second;
            int prevActionIndex = (int)prevStateAction.first;
            int prevQIndex = prevAction * 200 + prevActionIndex;

            //eval
            q[prevQIndex] = q[prevQIndex] + alpha * ((reward + discount * q[currQIndex]) - q[prevQIndex]);
            //improve
            previousAction = _actions[prevActionIndex];
            _updated = _updated || (previousAction != (_actions[prevActionIndex] = q[prevActionIndex] > q[prevActionIndex + 200] ? false : true));
        }
        else if (isFinal)
        {
            //eval
            q[currQIndex] = q[currQIndex] + alpha * (reward - q[currQIndex]);
            //improve
            previousAction = _actions[currActionIndex];
            _updated = _updated || (previousAction != (_actions[currActionIndex] = q[currActionIndex] > q[currActionIndex + 200] ? false : true));
        }

        return _updated;
    }

    override bool GetAction(int currentSum, bool hasUsableAce, int dealerCard)
    {
        Action currentAction = base::GetAction(currentSum, hasUsableAce, dealerCard);

        EvaluateAndImprovePolicy(0.0, false);

        return currentAction;
    }
};
}
