#include "utility/filters/AnalogFilter.hpp"
#include <algorithm>
#include <iterator>

using namespace utility;

#define MAX_ORDER 8

/****************************************************************************
 * Copyright (C) 2022 Altair Lab
 * @author Altair Lab
 * @date Mar 2022
 ****************************************************************************/

// Class constructor
AnalogFilter::AnalogFilter(int filter_order, double *a, double *b)
{
    order = std::min(filter_order, MAX_ORDER);

    alpha = std::vector<double>(order + 1, 0.0);
    beta = std::vector<double>(order + 1, 0.0);
    beta_hat = std::vector<double>(order + 1, 0.0);

    num = std::vector<double>(order + 1, 0.0);
    den = std::vector<double>(order + 1, 0.0);

    x = std::vector<double>(order, 0.0);
    x_old = std::vector<double>(order, 0.0);
    f = std::vector<double>(order, 0.0);

    for (int i = order; i >= 0; i--)
    {
        if (b[i] != 0.0)
            num_order = order - i;

        if(a[i] != 0.0)
            den_order = order - i;

        num[i] = b[i];
        den[i] = a[i];
    }

    std::reverse(num.begin(), num.end());
    std::reverse(den.begin(), den.end());

    for (int i = 0; i <= order; i++)
    {
        alpha[i] = a[i] / a[0]; // attention!! problems when a[0]=0!!
        beta[i] = b[i] / a[0];
    }

    // std::reverse(std::begin(alpha), std::end(alpha));
    // std::reverse(std::begin(beta), std::end(beta));
    std::reverse(alpha.begin(), alpha.end());
    std::reverse(beta.begin(), beta.end());

    beta_hat[order] = beta[order];
    for (int i = 0; i < order; i++)
    { // todo <=???
        beta_hat[i] = beta[i] - alpha[i] * beta_hat[order];
    }

    y = 0;
}

void AnalogFilter::clean_F()
{
    for (int i = 0; i < order; i++)
    {
        f[i] = 0.0;
    }

    y = 0;
}

void AnalogFilter::clean()
{
    for (int i = 0; i < order; i++)
    {
        x[i] = 0.0;
        x_old[i] = 0.0;
        f[i] = 0.0;
    }

    y = 0;
}

// Fn = f[order - 1] -> E.g.: order = 2 -> F2 = f[1]; X2 = x[1]
double AnalogFilter::process(double input, double h)
{
    clean_F();

    for (int i = 0; i < order; i++)
    {
        f[order - 1] = f[order - 1] - alpha[i] * x[i];
    }

    f[order - 1] = f[order - 1] + input;
    x[order - 1] = x_old[order - 1] + h * f[order - 1];
    x_old[order - 1] = x[order - 1];

    y = beta_hat[order - 1] * x[order - 1] + beta_hat[order] * input;

    for (int i = order - 2; i >= 0; i--)
    {
        f[i] = x[i + 1];
        x[i] = x_old[i] + h * f[i];
        x_old[i] = x[i];

        y = y + beta_hat[i] * x[i];
    }

    return y;
}

AnalogFilter *AnalogFilter::getDifferentiatorHz(double f)
{
    double b[2] = {1.0, 0.0};                  // num
    double a[2] = {1.0 / (2 * M_PI * f), 1.0}; // den
    // double b[2] = {2*M_PI*f, 0.0}; //num
    // double a[2] = {1.0, (2*M_PI*f)}; // den
    return new AnalogFilter(1, a, b);
}

AnalogFilter *AnalogFilter::getLowPassFilterHz(double f)
{
    double b[2] = {0.0, 2 * M_PI * f}; // num
    double a[2] = {1.0, 2 * M_PI * f}; // den
    return new AnalogFilter(1, a, b);
}

AnalogFilter *AnalogFilter::getDifferentiatorHz2(double f)
{
    double wn = 2 * M_PI * f;
    double b[3] = {0.0, wn * wn, 0.0};    // num
    double a[3] = {1.0, 2 * wn, wn * wn}; // den
    return new AnalogFilter(2, a, b);
}

double AnalogFilter::getMagnitudeHz(float f)
{
    // Set the frequency in [rad/s]
    float w = 2 * M_PI * f;
    double magNum = 1.0f;
    double magDen = 1.0f;
    
    double realSum = 0.0f;
    double complexSum = 0.0f;

    for(int i = num_order; i > 0; i--) {
        // Real
        if (i % 2 == 0) {
            realSum +=  pow(-1,(i / 2)) * num[i] * pow(w,i);  
        } 
        else {  // Complex
            complexSum += pow(-1,((i + 1) / 2)) * num[i] * pow(w , i);
        }
    }
    realSum += num[0];
    magNum = sqrt(pow(realSum,2) + pow(complexSum,2));

    realSum = 0.0f;
    complexSum = 0.0f;
    for(int i = den_order; i > 0; i--) {
        // Real
        if (i % 2 == 0) {
            realSum +=  pow(-1,(i / 2)) * den[i] * pow(w,i);  
        } 
        else {  // Complex
            complexSum += pow(-1,((i + 1) / 2)) * den[i] * pow(w , i);
        }
    }
    realSum += den[0];
    magDen = sqrt(pow(realSum,2) + pow(complexSum,2));

    return magNum / magDen;
}