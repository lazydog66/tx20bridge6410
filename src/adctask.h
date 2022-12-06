#pragma once

#include <Arduino.h>


// Change this to the clock frequency of the Arduino;
constexpr uint32_t k_arduino_frequency = 16000000;

// This is the frequency that timer 1 is set to run at.
constexpr uint32_t k_timer_1_frequency = 5000;

// 16000000 / (64 * (k_timer_1_comapre_to + 1));

// This is the maximum sampling rate.
constexpr uint32_t k_adc_sample_rate = k_timer_1_frequency;

// //
// // Set the current adc task.
// //
// // The background adc task isr calls the task whenever it has a new sample.
// //
// void set_adc_task(class adctask *task);

// //
// // Clear the current adc task.
// //
// // This will clear the task only if it is the current task.
// //
// void clear_adc_task(class adctask *task);

//
// This is the adc task.
//
// The adc runs continuously in the background. The task hooks into the
// adc loop and acquires the samples as they are generated.
class adctask
{

 public:
  // Construct the adc task with a particular filter.
  // Note, the adc task gets to own the filter pointer.
  adctask(uint8_t adc_pin);
  ~adctask();

  // Return the sampling frequency used for the current sample set.
  // At the moment, the sampling rate is fixed.
  uint32_t sample_rate() const { return k_adc_sample_rate; }

  // Start this task
  virtual bool start() = 0;

  // Stop this task.
  virtual void stop() = 0;

  // Service the task.
  // Called by the background adc isr when each sample has been acquired.
  virtual void service(uint8_t sample_value) = 0;

 protected:
  // Start an adc task
  static void start_task(adctask* adc_task, uint8_t pin);

  // Stop an adc task if it is current.
  static void stop_task(adctask* adc_task = nullptr);

  // Which adc channel to sample on.
  uint8_t adc_pin_ = 0;
};
