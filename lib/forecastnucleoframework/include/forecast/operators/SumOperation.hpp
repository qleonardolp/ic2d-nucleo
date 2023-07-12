#ifndef OPERATOR_SUM_HPP
#define OPERATOR_SUM_HPP

#include "../Operator.hpp"

namespace forecast
{
    /**
     * @brief Sum operator class
     **/

    class SumOp : public Operator
    {
    public:
        /**
         * @brief Construct a new object. This constructor initialize,
         * the controller.
         */
        SumOp();

        virtual std::vector<float> process(const IHardware *hw,
                                           std::vector<float> input0,
                                           std::vector<float> input1) override;
    };

    inline OperatorFactory::Builder make_sum_op_builder()
    {
        auto fn = [](std::vector<float> params) -> Operator *
        {
            return new SumOp();
        };
        return {fn, {}, {"Input 1", "Input 2"}};
    }
}

#endif // OPERATOR_SUM_HPP