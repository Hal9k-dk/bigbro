#pragma once

/// Class used to handle and interpret readings from the current sensor
class Current
{
public:
    static const int NUM_SAMPLES = 100;
    
    /**
     * @param[in] pin The ADC pin used to read the current sensor.
     * @param[in] debug_pin Pin is pulsed on each sample. Disabled if < 0.
     * @param[in] threshold The Peak to Peak current threshold for when the printer is considered printing. [mA]
     */
    Current(uint8_t pin, int8_t debug_pin = -1, uint16_t threshold = 200);

    /**
     * @return Peak to peak current reading, with offset error compensated.
     */
    int read();

    /// Toggles debugpin and calls sample
    void handle();

    /**
     * @return true if the value from read has been above m_threshold within m_max_below_time.
     * @return true if the value from read has been below m_threshold for m_max_below_time or more.
     */
    bool is_printing();

    /// Sets m_last_above_thresh to be m_max_below_time ago, effectively clearing any recent above pulses.
    void reset_last_above();

    /// Sets all samples in the raw buffer to 0
    void clear_buffer();

    /**
     * This function should only be run when the relay is open, and the actual current is 0A.
     * It'll take a collection of samples, and store the average as the error offset.
     */
    void calibrate();

    /**
     * The sensor is bidirectional, so idles at 2.5v with 0A.
     * @returns true if the voltage is between ~2.2v and ~2.7v
     * @returns false otherwise
     */
    bool sensor_present(); // Checks if there's a sensor on the board.

    /**
     * Reads a sample into m_raw_samples, wrapping around when filled
     * Only reads a new sample if at least m_sample_period has passed.
     */
    void sample();        

    int get_raw_sample(int index) const;

    int get_raw_value() const;
    
private:
    
    /// Analog pin connected to current sensor
    uint8_t m_pin = 0;

    /// Optional pin used if a debug_pin is supplied
    int16_t m_debug_pin = 0;

    /// [ms]
    uint32_t m_last_sample		= 0;
    /// [ms]
    uint8_t m_sample_period     = 10;
    /// Array of raw ADC samples
    int16_t m_raw_samples[NUM_SAMPLES];
    /// Index of the next sample address 
    uint8_t m_raw_sample_offset	= 0;
    /// Size of the sample buffer in elements
    uint8_t m_raw_sample_size 	= 100;
    /// The offset at 0A. set by a calibrate run
    int16_t m_error				= 0;

    /// The voltage at full range ADC reading [mV]
    const uint16_t m_v_range	= 4650;
    /// The mV per A
    const uint16_t m_mv_per_A   = 200;
    /// The conversion factor from raw to mA [mA/ADCstep]
    const float raw_to_mA_factor = m_v_range/1024.0 * m_mv_per_A/1000.0;

    /// Printer is considered printing above this threshold
    int m_threshold = 0;

    /// Last time a reading was above the threshold [ms]
    uint32_t m_last_above_thresh = 0;

    /// Maximum time the reading is allowed to be under the threshold [ms]
    uint32_t m_max_below_time   = 60000; 

    /**
     * @return largest peak-to-peak raw reading
     */
    int m_p2p();
};
