#include <forecast/operators/SumOperation.hpp>

using namespace forecast;

SumOp::SumOp(){};

std::vector<float> SumOp::process(const IHardware *hw, std::vector<float> input0, std::vector<float> input1)
{
    return {input0[0] + input1[0]};
}