#ifndef __POLICY_HPP__
#define __POLICY_HPP__

#include <iostream>
#include <vector>

namespace object_model
{
class Policy
{
protected:
  constexpr static bool _actions[200] = {0};
  constexpr static double q[400] = {0};
  constexpr static int timesVisited[400] = {0};
  std::vector<std::pair<int, bool>> _history;
  const double epsilon = 0.1;
  constexpr static double alpha = 0.001;
  const double discount = 0.9;

public:
  Policy();

  virtual void ClearHistory();

  virtual bool GetAction(int currentSum, bool hasUsableAce, int dealerCard);

  ///returns true if best action for any state is changed
  virtual bool EvaluateAndImprovePolicy(double reward, bool isFinal = true) = 0;

  static void FlushToDisk();
};
}

#endif // __POLICY_HPP__