#ifndef ANALOGFILTER_H
#define ANALOGFILTER_H

#include <cmath>
#include <vector>
#include "Filter.hpp"

#define __sign(x) (std::signbit(x) ? -1.0 : 1.0)

namespace utility
{
    /****************************************************************************
     * Copyright (C) 2022 Altair Lab
     * @author Altair Lab
     * @date Mar 2022
     ****************************************************************************/

    /**
     * This class implements a Laplace filter using the matlab notation:
     * * b -> array of numerator coefficients of L transfer function
     * * a -> array of denominator coefficients of L transfer function
     * The left value of arrays a and b are the z^n coefficients es.:
     * [a2 a1 a0] -> a2*s^2 + a1*s + a0
     * Remark 1 : the maximum filter order is set by the static proprerty
     * MAX_ORDER (= 8 by default)
     */

    class AnalogFilter : public Filter
    {
    public:
        /**
         * \brief   Analog Filter constructor based on Laplace transfer function
         *          coefficients
         * \param order Order of the filter (the maximum filter order is set
         *              by the static property MAX_ORDER = 8 by default)
         * \param a     array of denominator coefficients of Laplace transfer
         *              function
         * \param b     array of numerator coefficients of Laplace transfer
         *              function
         */
        AnalogFilter(int order, double *a, double *b);
        virtual ~AnalogFilter() {}

        /// @brief data process, can be used even in the presence of jitter
        double process(double input, double h);
        /// @brief cleans up the filter internal states
        void clean();
        /// @brief return the magnitude of the transfer function at a specified frequency in Hz
        double getMagnitudeHz(float f);

        /**
         * @brief   builds an approximate differentiator filtering with cut-off
         *          frequency f [Hz]
         */
        static AnalogFilter *getDifferentiatorHz(double f);

        static AnalogFilter *getLowPassFilterHz(double f);

        /**
         * @brief   builds an approximate differentiator filtering with cut-off
         *          frequency f [Hz] (second order low pass)
         */
        static AnalogFilter *getDifferentiatorHz2(double f);

    private:
        int order;     ///< the filter order
        int num_order = 0; ///< the order of the numerator
        int den_order = 0; ///< the order of the denominator

        std::vector<double> num;
        std::vector<double> den;

        std::vector<double> alpha;
        std::vector<double> beta;
        std::vector<double> beta_hat;

        std::vector<double> x;
        std::vector<double> x_old;
        std::vector<double> f;

        double y;

        void clean_F(); ///< cleans up the derivative vector F
    };
}
#endif // ANALOGFILTER_H
