#include "../include/utility/filters/DigitalFilter.hpp"
using namespace utility;

/****************************************************************************
 * Copyright (C) 2015 Andrea Calanca
 * @author Andrea Calanca
 * @date june 2015
 ****************************************************************************/

// class constructor
DigitalFilter::DigitalFilter(int filter_order, double* ap, double* bp)
{
	//to simplify things I use the filter order plus one
    this->order_p1 = filter_order+1;

    for (int i=0;i<order_p1;i++)
    {
        a[i] = ap[i];
        b[i] = bp[i];
        u[i] = 0.0;
        y[i] = 0.0;
    }
    k = 0;
}

void DigitalFilter::clean()
{
    for (int i=0;i<order_p1;i++)
    {
        u[i] = 0.0;
        y[i] = 0.0;
    }
    k = 0;
}

double DigitalFilter::process(double input)
{
	// remind: order_p1 is the filter order PLUS ONE!!

    int i,h=0;

    k++;
    k = k % order_p1;

    u[k] =  input;
    y[k] =  b[0]*u[k];

	//matlab:
    //a(1)*y(n) =   b(1)*x(n) + b(2)*x(n-1) + ... + 
    //              b(nb+1)*x(n-nb) - a(2)*y(n-1) - ... - a(na+1)*y(n-na)
	//with our notation:
    //a(0)*y(k) =   b(0)*u(k) + b(1)*u(k-1) + ... + 
    //              b(nb)*x(k-nb) - a(1)*y(k-1) - ... - a(na)*y(k-na)

    for (i=1;i<order_p1;i++)
    {
        h = k - i + order_p1;
        h = h % order_p1;
        y[k] += b[i]*u[h] - a[i]*y[h];
    }

    y[k] = y[k] / a[0];

    return y[k];
}

DigitalFilter* DigitalFilter::getLowPassFilterHz(double f, double TS)
{
	double alpha = exp(-2*M_PI*f*TS);
	double b[2] = {(1-alpha),0.0}; //num
    double a[2] = {1.0,-alpha}; // den
    return new DigitalFilter(1,a,b);
}

DigitalFilter* DigitalFilter::getDifferentiatorHz(double f, double TS)
{
	double alpha = exp(-2*M_PI*f*TS);
	double b[2] = {1-alpha, alpha-1}; //num
    double a[2] = {TS,-alpha*TS}; // den
    return new DigitalFilter(1,a,b);
}

//this is a 2nd order filter. Better or worse than simplectic?
DigitalFilter* DigitalFilter::getResonatorHz(double f,double r, double TS)
{
	int g;
	g = (1.0-r*r) / (1.0+r*r - 2.0*r*cos(2.0*M_PI*f*TS));
	double cb[3] = {(1-r*r)*0.5/g, 0.0, (1-r*r)*0.5/g}; //num
    double ca[3] = {1.0, - 2.0*r*cos(2.0*M_PI*f*TS), r*r}; // den
	return new DigitalFilter(2,ca,cb);
}

DigitalFilter* DigitalFilter::getDifferentiator(double TS)
{
    double b[2] = {1,-1}; //num
    double a[2] = {TS,0}; // den
    return new DigitalFilter(1,a,b);
}

/* DigitalFilter* DigitalFilter::getSecondOrderLowPassFilterHz(
    double psi,
    double f
){
	return NULL;
} */

/* DigitalFilter* DigitalFilter::getSecondOrderFilterHz(
    double psi_p, 
    double f_p, 
    double psi_z,
    double f_z
){
	return NULL;
} */

/* DigitalFilter* DigitalFilter::getSecondOrderFilterFromLaplace(
    double a0, double a1, double a2, 
    double b0, double b1, double b2
){
	return NULL;
} */

/* DigitalFilter* DigitalFilter::getThirdOrderFilterFromLaplace(
    double a0, double a1, double a2, double a3, 
    double b0, double b1, double b2, double b3
){
	return NULL;
} */

DigitalFilter * DigitalFilter::getFiniteDiffOrd1Acc2(double h)
{
    double b[3] = {3.0/2.0, -2.0, 1.0/2.0}; // num
    double a[3] = {0.0, 0.0, h}; // den

    return new DigitalFilter(2,a,b);
}

DigitalFilter * DigitalFilter::getFiniteDiffOrd1Acc3(double h)
{
    double b[4] = {-1.0/3.0, 3.0/2.0, -3.0, 11.0/6.0}; // num
    double a[4] = {0.0, 0.0, 0.0, h}; // den

    return new DigitalFilter(3,a,b);
}

DigitalFilter * DigitalFilter::getFiniteDiffOrd1Acc4(double h)
{
    double b[5] = {1.0/4.0, -4.0/3.0, 3.0, -4.0, 25.0/12.0}; // num
    double a[5] = {0.0, 0.0, 0.0, 0.0, h}; // den

    return new DigitalFilter(4,a,b);
}

DigitalFilter * DigitalFilter::getFiniteDiffOrd2Acc2(double h)
{
    double b[4] = {-1.0, 4.0, -5.0, 2.0}; // num
    double a[4] = {0.0, 0.0, 0.0, h*h}; // den

    return new DigitalFilter(3,a,b);
}

DigitalFilter * DigitalFilter::getFiniteDiffOrd2Acc3(double h)
{
    double b[5] = {11.0/12.0, -14.0/3.0, 19.0/2.0, -26.0/3.0, 35.0/12.0}; // num
    double a[5] = {0.0, 0.0, 0.0, 0.0, h*h}; // den

    return new DigitalFilter(4,a,b);
}

DigitalFilter * DigitalFilter::getFiniteDiffOrd2Acc4(double h)
{
    // numerator
    double b[6] = {-5.0/6.0, 61.0/12.0, -13.0, 107.0/6.0, -77.0/6.0, 15.0/4.0}; 
    double a[6] = {0.0, 0.0, 0.0, 0.0, 0.0, h*h}; // den

    return new DigitalFilter(5,a,b);
}

DigitalFilter * DigitalFilter::getSmoothDifferentiator(int n, double h)
{
    double a[n+1];
    double b[n+1];

    // COEFFICIENTS COMPUTING

    std::vector<double> c_k, prev_c_k;
    c_k.resize(n+1);
    prev_c_k.resize(n+1);
    
    // Iterative procedure for coefficents computing
    c_k[0] = 1;
    for (int row = 1; row < n; row++)
    {
        prev_c_k = c_k;

        for (int col=1;  col < ((row+2)/2); col++ )
            c_k[col] = prev_c_k[col-1] + prev_c_k[col];
    }

    // Mirroring of the integer sequence
    for (int i = 0; i < n+1/2; i++)
        c_k[n-i] = -c_k[i];
    
    // DENOMINATOR
    a[0] = (double) pow(2, n-1)*h;
    for (int i = 1; i < n+1; i++)
        a[i] = 0.0;

    // NUMERATOR
    // Conversion from vector to double array
    for (unsigned int i = 0; i < c_k.size(); i++) 
        b[i] = c_k[i];
    
    return new DigitalFilter(n, a, b);
}

DigitalFilter* DigitalFilter::getDelay0()
{
    double b[1] = {1}; //num
    double a[1] = {1}; // den
    return new DigitalFilter(0,a,b);
}

DigitalFilter* DigitalFilter::getDelay1()
{
    double b[2] = {0,1}; //num
    double a[2] = {1,0}; // den
    return new DigitalFilter(1,a,b);
}

DigitalFilter* DigitalFilter::getDelay2()
{
    double b[3] = {0,0,1}; //num
    double a[3] = {1,0,0}; // den
    return new DigitalFilter(2,a,b);
}

DigitalFilter* DigitalFilter::getDelay3()
{
    double b[4] = {0,0,0,1}; //num
    double a[4] = {1,0,0,0}; // den
    return new DigitalFilter(3,a,b);
}

DigitalFilter* DigitalFilter::getDelay4()
{
    double b[5] = {0,0,0,0,1}; //num
    double a[5] = {1,0,0,0,0}; // den
    return new DigitalFilter(4,a,b);
}

DigitalFilter* DigitalFilter::getDelay5()
{
    double b[6] = {0,0,0,0,0,1}; //num
    double a[6] = {1,0,0,0,0,0}; // den
    return new DigitalFilter(5,a,b);
}


DigitalFilter* DigitalFilter::getMean10Filter()
{
    double n = 10;
    double b[10] = {1,1,1,1,1,1,1,1,1,1}; //num
    double a[10] = {n,0,0,0,0,0,0,0,0,0}; // den
    return new DigitalFilter(n,a,b);
}


DigitalFilter* DigitalFilter::get10HzButtFilter()
{
    double b[2] = {0.0052,0.0052}; //num
    double a[2] = {1.0,-0.9896}; // den
    return new DigitalFilter(1,a,b);
}

DigitalFilter* DigitalFilter::get20HzButtFilter()
{
    double b[2] = {0.0104,0.0104}; //num
    double a[2] = {1.0,-0.9793}; // den
    return new DigitalFilter(1,a,b);
}

DigitalFilter* DigitalFilter::get100HzButtFilter()
{
    double b[2] = {0.0498,0.0498}; //num
    double a[2] = {1.0,-0.9004}; // den
    return new DigitalFilter(1,a,b);
}

DigitalFilter* DigitalFilter::get1000HzButtFilter()
{
    double b[2] = {0.3660,0.3660}; //num
    double a[2] = {1.0,-0.2679}; // den
    return new DigitalFilter(1,a,b);
}
