#ifndef __SARSA_HPP__
#define __SARSA_HPP__

#include <iostream>
#include <vector>
#include "policy.h"

namespace object_model
{
class Sarsa : Policy
{
  private:
    bool _updated;

  public:
    void ClearHistory();

    bool EvaluateAndImprovePolicy(double reward, bool isFinal = true);

    bool GetAction(int currentSum, bool hasUsableAce, int dealerCard);
};
}

#endif // __SARSA_HPP__