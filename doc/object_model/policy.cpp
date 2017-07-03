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
class Policy
{

  public:
    Policy()
    {
        srand(time(NULL));
    }

    virtual void ClearHistory()
    {
        _history.clear();
    }

    virtual bool GetAction(int currentSum, bool hasUsableAce, int dealerCard)
    {
        int ace = hasUsableAce ? 1 : 0;
        int index = ace * 100 + (dealerCard % 10) * 10 + currentSum % 10;
        bool a = _actions[index];

        double r = ((double)rand() / (RAND_MAX));

        if (/*Program.Mode == Mode.Train && */ r < epsilon)
        {
            double randAction = ((double)rand() / (RAND_MAX));
            a = randAction > 0.5;
        }

        timesVisited[(int)a * 200 + index]++;

        _history.push_back(hasUsableAce * 100 + (dealerCard % 10) * 10 + currentSum % 10, a);
        return a;
    }

    ///returns true if best action for any state is changed
    virtual bool EvaluateAndImprovePolicy(double reward, bool isFinal = true) = 0;

    static void FlushToDisk()
    {
        // string text = string.Join(Environment.NewLine, timesVisited);

        // text += Environment.NewLine + string.Join(Environment.NewLine, q);
        // File.WriteAllText(Program.FileName, text);
    }
}
}