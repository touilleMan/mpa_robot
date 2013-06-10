/**
** \file robot.h
** \brief This file contains the functions the robot will use
**        to be interfaced with the emulator.
** \author Emmanuel Leblond, Aiden Kim
** \date 04/04/2012
*/

#ifndef ROBOT_H_
# define ROBOT_H_

typedef int (func_pool)(int *);

int robot_connect(func_pool *set_r_motor,
		  func_pool *set_l_motor,
		  func_pool *set_leds,
		  func_pool *get_sensors);
int robot_frame(float dt);

#endif /* !ROBOT_H_ */
