#ifndef __POLICY_HPP__
#define __POLICY_HPP__

#include <iostream>
#include <vector>
#include <fstream>
#include "state.h"

#define NUMBER_OF_STATES 1800
#define PRINT_LAST_STATEACTIONS 5
#define EXPLORATION 20


namespace object_model
{
class Policy
{
protected:
  Action _actions[NUMBER_OF_STATES];
  double q[NUMBER_OF_STATES * NUMBER_OF_ACTIONS];
  std::vector<std::pair<State, Action>> _history;
  double epsilon = 1 / EXPLORATION;
  double alpha = 0.01;
  const double discount = 0.8;

public:
  Policy();

  virtual ~Policy();
  virtual void ClearHistory();

  virtual Action GetAction(State s);

  ///returns true if best action for any state is changed
  virtual bool EvaluateAndImprovePolicy(double reward, bool isFinal) = 0;

  virtual void FlushToDisk(char* filename);

  virtual void LoadFromDisk(char* filename);
  void PrintHistory();
};
}

#endif // __POLICY_HPP__