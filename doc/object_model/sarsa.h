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
  Sarsa()
      : Policy(){};
  void ClearHistory();

  bool EvaluateAndImprovePolicy(double reward, bool isFinal);

  Action GetAction(State s);

  void FlushToDisk(char *filename);

  void LoadFromDisk(char *filename);
  
  void SetEpsilon(double eps) { epsilon = eps; };
};
}

#endif // __SARSA_HPP__