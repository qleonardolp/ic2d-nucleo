#ifndef DIGITALFILTER_H
#define DIGITALFILTER_H

#include <cmath>
#include <vector>

#include "Filter.hpp"

namespace utility {
	/***************************************************************************
	 * Copyright (C) 2015 Andrea Calanca
	 * @author Andrea Calanca
	 * @date june 2015
	 **************************************************************************/

	/**
	 * This class implements a digital filter using the matlab notation:
	 * b -> array of numerator coefficients of Z transfer function
	 * a -> array of denominator coefficients of Z transfer function
	 * The left value of arrays a and b are the z^n coefficients es.:
	 * [a2 a1 a0] -> a2*z^2 + a1*z + a0
	 * Remark 1 : the method process(double input) is supposed to be called 
	 * every period T, where T is the sampling time.
	 * Remark 2 : the maximum filter order is set by the statis proprierty 
	 * MAX_ORDER (= 8 by default)
	 */

	class DigitalFilter : public Filter
	{
	public:

		/** \brief Digital Filter constructor based on Z-transform coefficients
		 *	\param order Order of the filter (the maximum filter order is set by 
		           the statis proprierty MAX_ORDER = 8 by default)
		 *	\param a array of denominator coefficients of Z transfer function
		 *	\param b array of numerator coefficients of Z transfer function
		 */
		DigitalFilter(int order, double* a, double* b);
		virtual ~DigitalFilter() {}

		double process(double input, double h) 
		{
			// Ignore the h param 
			return process(input); 
		}

		/// @brief data process, to be called without jitter
		double process(double input);

		/// @brief cleans up the filter internal state
		void clean(); 

		/// @brief gets the last processed data
		double getValue(){ return y[k];} 
		
		static const int MAX_ORDER = 20;

		/** @brief 	builds a one pole low-pass filter with cut-off 
		 * 			frequency f [Hz]
		 */
		static DigitalFilter* getLowPassFilterHz(double f, double TS);

		/**
		 * @brief 	builds an approximate differentiator filter with cut-off 
		 * 			frequency f [Hz]
		 * 
		 */
		static DigitalFilter* getDifferentiatorHz(double f, double TS);

		/**
		 * @brief builds a resonator filter at frequency f [Hz]
		 * 
		 * @param f 
		 * @param r 
		 * @param TS 
		 * @return DigitalFilter* 
		 */
		static DigitalFilter* getResonatorHz(double f,double r, double TS);  

		/**
		 * @brief	builds a second order filter with two poles and 
		 * 			unitary static gain
		 */
		static DigitalFilter* getSecondOrderLowPassFilterHz(
			double psi,
			double f
		);

		
		/// @brief builds a second order filter with unitary static gain
		static DigitalFilter* getSecondOrderFilterHz(
			double psi_p, 
			double f_p, 
			double psi_z,
			double f_z
		);

		/**
		 * @brief 	builds a second order filter from transfer function 
		 *       	coefficients in Laplace domain. 
		 * 
		 * @param a0 first denominator coefficients
		 * @param a1 second denominator coefficients
		 * @param a2 third denominator coefficients
		 * @param b0 first numerator coefficients
		 * @param b1 second numerator coefficients
		 * @param b2 third numerator coefficients
		 * @return DigitalFilter* 
		 */
		static DigitalFilter* getSecondOrderFilterFromLaplace(
			double a0, 
			double a1, 
			double a2, 
			double b0, 
			double b1, 
			double b2
		);

		/**
		 * @brief 	builds a third order filter from transfer function 
		 * 			coefficients in Laplace domain.
		 * 
		 * @param a0 first denominator coefficient 
		 * @param a1 second denominator coefficient
		 * @param a2 third denominator coefficient
		 * @param a3 fourth denominator coefficient
		 * @param b0 first numerator coefficient 
		 * @param b1 second numerator coefficient
		 * @param b2 third numerator coefficient
		 * @param b3 fourth numerator coefficient
		 * @return DigitalFilter* 
		 */
		static DigitalFilter* getThirdOrderFilterFromLaplace(
			double a0,
			double a1, 
			double a2, 
			double a3, 
			double b0,
			double b1,
			double b2, 
			double b3
		);

		static DigitalFilter* getSmoothDifferentiator(int n, double h);

		DigitalFilter * getFiniteDiffOrd1Acc2(double h);
		DigitalFilter * getFiniteDiffOrd1Acc3(double h);
		DigitalFilter * getFiniteDiffOrd1Acc4(double h);
		DigitalFilter * getFiniteDiffOrd2Acc2(double h);
		DigitalFilter * getFiniteDiffOrd2Acc3(double h);
		DigitalFilter * getFiniteDiffOrd2Acc4(double h);

		static DigitalFilter* getDelay0();
		static DigitalFilter* getDelay1();
		static DigitalFilter* getDelay2();
		static DigitalFilter* getDelay3();
		static DigitalFilter* getDelay4();
		static DigitalFilter* getDelay5();
		static DigitalFilter* getDelay10();

		static DigitalFilter* getMean10Filter();

		/// @brief builds a finite difference filter
		static DigitalFilter* getDifferentiator(double TS); 

		/**
		 * @brief 	return a differentiator with a 20Hz pole - w.r.t. 
		 * 			differentiating and then filtering you can spare a dt
		 */
		// static DigitalFilter* get20HzDerivator(); 

		/**
		 * @brief 	return a differentiator with a 50Hz pole - w.r.t. 
		 * 			differentiating and then filtering you can spare a dt 
		 */
		// static DigitalFilter* get50HzDerivator();

		/// @brief supposes T = 1 / 3kHz
		static DigitalFilter* get10HzButtFilter();
		/// @brief supposes T = 1 / 3kHz
		static DigitalFilter* get20HzButtFilter();
		/// @brief supposes T = 1 / 3kHz
		static DigitalFilter* get100HzButtFilter();
		/// @brief supposes T = 1 / 3kHz
		static DigitalFilter* get1000HzButtFilter();

	private:

		double a[MAX_ORDER];
		double b[MAX_ORDER];
		double u[MAX_ORDER];
		double y[MAX_ORDER];
		int k;
		int order_p1; ///< the filter order plus one

	};
}
#endif // DIGITALFILTER_H
