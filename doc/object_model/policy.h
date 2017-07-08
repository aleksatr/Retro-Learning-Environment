#ifndef __POLICY_HPP__
#define __POLICY_HPP__

#include <iostream>
#include <vector>
#include <fstream>
#include "state.h"

#define NUMBER_OF_STATES 2400
#define EXPLORATION 20


namespace object_model
{
class Policy
{
protected:
  Action _actions[NUMBER_OF_STATES];
  double q[NUMBER_OF_STATES * NUMBER_OF_ACTIONS];
  int times_visited[NUMBER_OF_STATES * NUMBER_OF_ACTIONS];
  std::vector<std::pair<State, Action>> _history;
  std::vector<double> reward_history;
  double epsilon = 1 / EXPLORATION;
  double alpha = 0.001;
  const double discount = 0.9;

public:
  Policy();

  virtual ~Policy();
  virtual void ClearHistory();

  virtual Action GetAction(State s);

  virtual void AddRewardToHistory(double reward);

  ///returns true if best action for any state is changed
  virtual bool EvaluateAndImprovePolicy(double reward, bool isFinal) = 0;

  virtual void FlushToDisk(char* filename);

  virtual void LoadFromDisk(char* filename);

  inline virtual void SetEpsilon(double eps) { epsilon = eps; };
};
}

#endif // __POLICY_HPP__