#ifndef __SARSA_HPP__
#define __SARSA_HPP__

#include <iostream>
#include <vector>
#include "policy.h"

namespace object_model
{
class Sarsa : public Policy
{
private:
  bool _updated;

public:
  Sarsa()
      : Policy(){};
  void ClearHistory();

  bool EvaluateAndImprovePolicy(double reward, bool isFinal);

  Action GetAction(State s);

  void SetEpsilon(double eps) { epsilon = eps; };

private:
  int EvalueTD(double reward, int index, int prevIndex, double gama);
};
}

#endif // __SARSA_HPP__