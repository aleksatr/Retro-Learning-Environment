#ifndef __MONTE_CARLO_HPP__
#define __MONTE_CARLO_HPP__

#include <iostream>
#include <vector>
#include "policy.h"

namespace object_model
{
    class MonteCarlo : public Policy
    {
public:
    MonteCarlo()
        : Policy(){};
        
    bool EvaluateAndImprovePolicy(double reward, bool isFinal);
    };
}

#endif // __MONTE_CARLO_HPP__