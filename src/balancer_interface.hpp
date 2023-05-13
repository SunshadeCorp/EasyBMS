#pragma once

#include <vector>

class IBalancer {
   public:
    virtual void balance(const std::vector<float>& voltages) = 0;
    virtual std::vector<bool> balance_bits() = 0;
};