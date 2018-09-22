#pragma once

class Current
{
    public:
		// pin = adc pin, sample_period = delay between samples in uS;
        Current(uint8_t pin, int8_t debug_pin = -1, uint16_t threshold = 200); 
        int16_t read();
        void handle();
        bool is_printing();
        void calibrate(); // Run this at 0A to collect the 0A offset.
        bool sensor_present(); // Checks if there's a sensor on the board.
		void sample();

    private:
        uint8_t m_pin;
        uint8_t m_debug_pin;

        uint32_t m_last_sample		= 0;
        uint8_t m_sample_period     = 10;
		bool    m_raw_buffer_filled	= 0;
		int16_t m_raw_samples[100]; 
    	uint8_t m_raw_sample_offset	= 0;
    	uint8_t m_raw_sample_size 	= 100;
    
        int16_t m_error				= 0;

        int16_t  m_AC_current		= 0;
        const uint16_t m_v_range	= 4650; // mV   | Max ADC voltage on the pin of the divider.
        const uint16_t m_mv_per_A   = 200;  // mV/A | 
        int16_t m_threshold         = 200;  // Printer is considered printing above this threshold

        uint32_t m_last_above_thresh;
        uint16_t m_max_below_time   = 100;  // ms

        uint16_t m_p2p();
};

