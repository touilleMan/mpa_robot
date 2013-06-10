/**
 ** \file emulator.hh
 ** \brief This file contains the functions the robot will use
 **        to be interfaced with the emulator.
 ** \author Emmanuel Leblond
 ** \date 04/04/2012
 */

#ifndef EMULATOR_H_
# define EMULATOR_H_

# include <SFML/Graphics.hpp>

extern "C"
{
// The robot is coded in C
# include "robot.h"
}

# define MAP_HEIGHT 800
# define MAP_WIDTH 600
# define DRAW_WIDE 12
# define RESSOURCES_ROBOT "../ressources/car.png"
# define RESSOURCES_SENSOR_ON "../ressources/sensor_on.png"
# define RESSOURCES_SENSOR_OFF "../ressources/sensor_off.png"
# define RESSOURCES_FONT "../ressources/arial.ttf"
# define SENSOR_TAB_X 30
# define SENSOR_TAB_Y 30
# define STR_SENSORS_X 80
# define STR_SENSORS_Y 5
# define STR_SENSOR_TXT "-- sensors --"
# define STR_R_MOTOR_X 10
# define STR_R_MOTOR_Y 550
# define STR_L_MOTOR_X 200
# define STR_L_MOTOR_Y 550
# define STR_LEGEND_X 600
# define STR_LEGEND_Y 10
# define STR_LEGEND_TXT \
" --- COMMANDS : ---\n\
space : display tracer\n\
arrows : turn robot\n\
right mouse : move robot\n\
left mouse : draw line\n\
F12 : print screen\n"


# define PRT_SCREEN "prt_"

# define TIME_SLICE 0.001F
# define ROBOT_ROT_SPEED 50
# define ROBOT_WHEEL 10
# define ROBOT_WIDTH 70
# define ROBOT_SENSOR_WIDTH 10

// Functions used by the C robot IA.
int set_r_motor(int *motor);
int set_l_motor(int *motor);
int set_leds(int *leds);
int get_sensors(int *sensors);

/**
 ** \class Emulator
 ** \brief This class is a singleton representing the emulated environment
 **        where the robot IA evolves.
 */
class Emulator
{
  public:
  static Emulator *get_instance(void);
  int run(void);

  private:
  Emulator(void);
  ~Emulator(void);
  void handle_events_(void);
  void refresh_screen_(void);
  void update_world_(float dt);

  static Emulator instance_;
  sf::RenderWindow window_;
  sf::Image img_map_;
  sf::Image img_tracer_;
  sf::Image img_sensor_on_;
  sf::Image img_sensor_off_;
  sf::Image img_robot_;
  sf::Sprite spr_map_;
  sf::Sprite spr_tracer_;
  sf::Sprite spr_robot_;
  sf::Sprite spr_sensors_[7];
  sf::Font font_;
  sf::String str_r_motor_;
  sf::String str_l_motor_;
  sf::String str_legend_;
  sf::String str_sensors_;

  bool stop_;
  bool tracer_;
  int prt_screen_count_;

  // leds and sensors are stored from left to right.
  int robot_sensors_[7];
  int robot_leds_[7];
  /*
   * Motor magnets state are stored in the array this way :
   *    +-----0-----+
   *    |           |
   *    |           |
   *    3           1
   *    |           |
   *    |           |
   *    +-----2-----+
   */
  int r_motor_[4];
  int l_motor_[4];
  int r_motor_prev_[4];
  int l_motor_prev_[4];
  float r_motor_real_;
  float l_motor_real_;
  float l_motor_clock_;
  float r_motor_clock_;

  friend int set_r_motor(int *motor);
  friend int set_l_motor(int *motor);
  friend int set_leds(int *leds);
  friend int get_sensors(int *leds);

};

#endif /* !EMULATOR_H_ */
