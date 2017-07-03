#ifndef __POLICY_HPP__
#define __POLICY_HPP__

#include <iostream>
#include <vector>
#include "state.h"

#define NUMBER_OF_STATES 5000

namespace object_model
{
class Policy
{
protected:
  Action _actions[NUMBER_OF_STATES];
  double q[NUMBER_OF_STATES * NUMBER_OF_ACTIONS];
  std::vector<std::pair<State, Action>> _history;
  const double epsilon = 0.1;
  double alpha = 0.001;
  const double discount = 0.9;

public:
  Policy();

  virtual ~Policy();
  virtual void ClearHistory();

  virtual Action GetAction(State s);

  ///returns true if best action for any state is changed
  virtual bool EvaluateAndImprovePolicy(double reward, bool isFinal) = 0;

  void FlushToDisk();
};
}

#endif // __POLICY_HPP__