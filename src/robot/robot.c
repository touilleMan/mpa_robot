#include "robot.h"
#include <stdio.h>

#define MOTOR_DEFAULT_COUNT 200

struct
{
  float motor_count_r;
  float motor_count_l;
  func_pool *set_r_motor;
  func_pool *set_l_motor;
  func_pool *set_leds;
  func_pool *get_sensors;
  int motor_l[4];
  int motor_r[4];
} emulator;

int robot_connect(func_pool *set_r_motor,
                  func_pool *set_l_motor,
                  func_pool *set_leds,
                  func_pool *get_sensors)
{
  emulator.motor_count_r = MOTOR_DEFAULT_COUNT;
  emulator.motor_count_l = MOTOR_DEFAULT_COUNT;
  emulator.set_r_motor = set_r_motor;
  emulator.set_l_motor = set_l_motor;
  emulator.set_leds = set_leds;
  emulator.get_sensors = get_sensors;

  emulator.motor_l[0] = 1;
  emulator.motor_r[0] = 1;
  emulator.motor_l[1] = 0;
  emulator.motor_r[1] = 0;
  emulator.motor_l[2] = 0;
  emulator.motor_r[2] = 0;
  emulator.motor_l[3] = 0;
  emulator.motor_r[3] = 0;

  return 0;
}

void set_speed(int coeff_l, int coeff_r)
{
  int i = 0;

  // Update the counter.
  emulator.motor_count_l -= coeff_l;
  emulator.motor_count_r -= coeff_r;

  // If one of the counters is down, update the motor magnets' positions.
  if (emulator.motor_count_l <= 0)
  {
    emulator.motor_count_l = MOTOR_DEFAULT_COUNT;
    for (i = 0; i < 4; ++i)
      if (emulator.motor_l[i] == 1)
      {
        emulator.motor_l[i] = 0;
        emulator.motor_l[(i + 1) % 4] = 1;
	break;
      }
    emulator.set_l_motor(emulator.motor_l);
  }

  if (emulator.motor_count_r <= 0)
  {
    emulator.motor_count_r = MOTOR_DEFAULT_COUNT;
    for (i = 0; i < 4; ++i)
      if (emulator.motor_r[i] == 1)
      {
        emulator.motor_r[i] = 0;
        emulator.motor_r[(i + 1) % 4] = 1;
	break;
      }
    emulator.set_r_motor(emulator.motor_r);
  }
}

int robot_frame(float dt)
{
  int i = 0;
  int leds[] = { 0, 0, 0, 0, 0, 0, 0 };
  int sensors[] = { 0, 0, 0, 0, 0, 0, 0 };
  int total_sensors[] = { 0, 0, 0, 0, 0, 0, 0 };

  for (i = 0; i < 7; ++i)
  {
    // Enable current led.
    leds[i] = 1;
    emulator.set_leds(leds);

    // Get the input measure.
    emulator.get_sensors(sensors);

    // Disable current led to save power.
    leds[i] = 0;
    emulator.set_leds(leds);

    // Save the state of the led we mesured.
    total_sensors[i] = sensors[i];
  }

  // Now compute the measures to determine the need speed for each motor.
  int coeff_l = 10;
  int coeff_r = 10;

  if (total_sensors[0])
    coeff_r += 10;
  else if (total_sensors[1])
    coeff_r += 5;
  else if (total_sensors[2])
    coeff_r += 3;

  if (total_sensors[6])
    coeff_l += 10;
  else if (total_sensors[5])
    coeff_l += 5;
  else if (total_sensors[4])
    coeff_l += 3;

  set_speed(coeff_l, coeff_r);

  return 0;
}
