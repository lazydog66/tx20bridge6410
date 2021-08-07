// --------------------------------------------------------------------------------------------------------------------
// Interface for the Davis 6410 wind meter.
//
// The wind meter measures wind speed and direction. The speed is measured by
// counting pulses prodcued by a reed switch, and the direction is encoded by a
// 20KOhm potentiometer.
// --------------------------------------------------------------------------------------------------------------------

/*
    This file is a part of SORT(Simple Open Ray Tracing), an open-source cross
    platform physically based renderer.
    Copyright (c) 2011-2020 by Jiayin Cao - All rights reserved.
    SORT is a free software written for educational purpose. Anyone can distribute
    or modify it under the the terms of the GNU General Public License Version 3 as
    published by the Free Software Foundation. However, there is NO warranty that
    all components are functional in a perfect manner. Without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.
    You should have received a copy of the GNU General Public License along with
    this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

#pragma once

#include <Arduino.h>

#include "windmeterintf.h"

// This is the default duration over which the wind speed is calculated.
// The anenometer's spec says the minimum wind speed is 1 mph which is 1
// revolution per 2.25 seconds, so 2.25 seconds seems like a reasonable amount
// of time. This has the advantage that the returned wind speed will be an
// integer number of 1 mph.
constexpr unsigned long k_wind_speed_sample_t = 2250;

// The state for the 6410.
//    idle - the 6410 is doing nothing
//    new_sample - a new sample has been requested
//    sampling_speed -
enum class davis6410state {
  idle,
  new_sample,
  sampling_speed,
  sampling_direction,
  send_frame,
};

class davis6410 : public windmeterintf {
 public:
  // The Davis runs off two pins, a digital input for the wind speed pulses and
  // an analogue pin for the wind direction. The anenometer's spec says the
  // minimum wind speed is 1 mph which is 1 revolution per 2.25 seconds, so
  // 2.25 seconds for the period has the advantage that the returned pulse count
  // is the wind speed in mph.
  davis6410(int wind_sensor_pin, int wind_direction_pin,
            unsigned long sample_period = 2250);

  // Initialise the hardware resources and set up the isr.
  // This must be done once before the 6410 can be used.
  void initialise();

  // Service the interface.
  void service();

  // Start a new sample.
  // The callback will be called when the sample is ready.
  // Returns true if the sample was started, false otherwise.
  bool start_sample(windsamplefn fn, void* context) override;

  // Abort the current sample if there is one in progress.
  void abort_sample() override;

  // Return the last sampled wind speed.
  float get_wind_mph() const override;

  // Return the last sampled wind direction.
  // Returns the direction as 0=N, E=4 etc.
  int get_wind_direction() const override;

  // Return the state of the Davis 6410.
  davis6410state state() const { return state_; }

  // Sample the wind speed.
  // The pulses from the Davis are counted over the sampling period and
  // converted to mph.
  // float sample_wind_mph() const;

  // // Sample the wind direction.
  // // Returns (0, 15) where 0 is North, 4 is East etc.
  // int sample_wind_direction() const;

 private:
  // Convert pulses to mph.
  // Note, this may in the future apply calibration data to the result.
  float calculate_wind_mph(uint8_t pulses) const;

  // A digital pin is used to counting the anenometer pulses.
  const int wind_speed_pin_;

  // The analog pin to use for reading the wind vane direction.
  const int wind_vane_pin_;

  // The duration in milliseconds of the sample period.
  unsigned long sample_period_;

  // The resources must be initialised before the 6410 can be read.
  bool initialised_ = false;

  // The state of the interface.
  davis6410state state_ = davis6410state::idle;

  // This is the start time in milliseconds of the current sample frame.
  unsigned long sample_start_time_;

  // This is the pulse count for the last sample frame.
  uint8_t sample_pulse_count_;

  // This is the last analogue reading for the wind direction.
  int sample_direction_;

  // The wind sample callback function.
  windsamplefn sample_fn_ = nullptr;

  // A context that is passed to the callback function.
  void* context_ = nullptr;
};
