#ifndef DIGITAL_ENCODER_H
#define DIGITAL_ENCODER_H

/**
 *  Abstract class used to implement different types of encoder
 */

class DigitalEncoder {
    public: // data types
    
    /**
     * @brief List the valid format the read angles
     * 
     * The format of an angle indicates how the class should behave when 
     * the hardware encoder overflows.
     */
    enum class AngleFormat {
        CONTINOUS = 0,  /**< The value of the angles overflows.
                                The value can be greater of 2*pi if the 
                                encoder performs more revolutions. */
        MOD2PI,          /**< The value of the angles if in module 2*pi 
                                so if the encoder performs an entire 
                                revolution the value return to 0 */
        LINEAR          /**< Format for linear encoders */
    };

    protected:
        /**
         * @brief Returns the row value from the encoder 
         */
        virtual int getAngle() = 0;
    
    public:
        /**
         * @brief Returns the max possible value from getAngle()
         */
        virtual int getMaxAngle() = 0;

        /**
         * @brief Reset the value of the encoder.
         * 
         * @param resetOffset the new value
         */
        virtual void reset(int resetOffset = 0) = 0;
        
        /**
         * @brief Returns the value of the angle in degree.
         * 
         * The value can be grater of 360 only if the angleFormat is 
         * CONTINUES
         */
        virtual double getAngleDeg();

        /**
         * @brief Returns the value of the angle in radians
         * 
         * The value can be grater of 2*pi only if the angleFormat is 
         * CONTINUES
         */
        virtual double getAngleRad();

        /**
         * @brief Returns the value of the current angleFormat
         */
        AngleFormat getAngleFormat();

        /**
         * @brief Set the angleFormat
         * 
         * @param angleFormat 
         */
        void setAngleFormat(AngleFormat angleFormat); 

        /**
         * @brief Set the value of the gear ratio of the encoder
         * 
         * @param gr Gear ratio value 
         */
        virtual void setGearRatio(float gr);

        /**
         * @brief Returns the value of the gear ratio 
         */
        virtual float getGearRatio();

    private:
        AngleFormat angleFormat = AngleFormat::CONTINOUS;

        /// contains the number of revolution occurred
        int rev_count; 

        /// contains the value of the gear ratio
        float gearRatio;
};
#endif
