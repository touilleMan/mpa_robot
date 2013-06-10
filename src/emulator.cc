#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>

#include "../config.h"
#include "emulator.hh"

#define PI 3.14159265

Emulator Emulator::instance_;

Emulator::Emulator(void)
  : window_(sf::VideoMode(800, 600), PACKAGE_STRING), stop_(false),
    tracer_(false), prt_screen_count_(0), r_motor_real_(0), l_motor_real_(0),
    r_motor_clock_(0), l_motor_clock_(0)
{
  for (int i = 0; i < 7; ++i)
  {
    robot_sensors_[i] = 0;
    robot_leds_[i] = 0;
  }

  for (int i = 0; i < 4; ++i)
  {
    r_motor_[i] = 0;
    l_motor_[i] = 0;
    r_motor_prev_[i] = 0;
    l_motor_prev_[i] = 0;
  }
}

Emulator::~Emulator(void)
{
}

Emulator *Emulator::get_instance(void)
{
  return &instance_;
}

int Emulator::run(void)
{
  sf::Clock clock;

  // Load the font and creat the strings
  sf::Font Arial;
  if (!Arial.LoadFromFile(RESSOURCES_FONT))
    return EXIT_FAILURE;
  str_r_motor_.SetPosition(STR_R_MOTOR_X, STR_R_MOTOR_Y);
  str_r_motor_.SetColor(sf::Color(0, 0, 0, 255));
  str_r_motor_.SetSize(20);
  str_l_motor_.SetPosition(STR_L_MOTOR_X, STR_L_MOTOR_Y);
  str_l_motor_.SetColor(sf::Color(0, 0, 0, 255));
  str_l_motor_.SetSize(20);

  str_sensors_.SetPosition(STR_SENSORS_X, STR_SENSORS_Y);
  str_sensors_.SetText(STR_SENSOR_TXT);
  str_sensors_.SetColor(sf::Color(0, 0, 0, 255));
  str_sensors_.SetSize(20);

  str_legend_.SetPosition(STR_LEGEND_X, STR_LEGEND_Y);
  str_legend_.SetText(STR_LEGEND_TXT);
  str_legend_.SetColor(sf::Color(0, 0, 0, 255));
  str_legend_.SetSize(15);

  // Create the map.
  img_map_.Create(MAP_HEIGHT, MAP_WIDTH, sf::Color(255, 255, 255, 255));
  img_tracer_.Create(MAP_HEIGHT, MAP_WIDTH, sf::Color(0, 0, 0, 0));

  // Initialize the robot to work through the emulator.
  robot_connect(&set_r_motor, &set_l_motor, &set_leds, &get_sensors);
  // Initialize the robot sprite
  if (!img_robot_.LoadFromFile(RESSOURCES_ROBOT))
    return EXIT_FAILURE;
  spr_robot_.SetImage(img_robot_);
  spr_robot_.SetCenter(img_robot_.GetWidth() / 2, img_robot_.GetHeight() / 2);
  spr_robot_.Move(img_map_.GetWidth() / 2, img_map_.GetHeight() / 2);

  // Initialize the sprites for the sensors.
  if (!img_sensor_on_.LoadFromFile(RESSOURCES_SENSOR_ON))
    return EXIT_FAILURE;
  if (!img_sensor_off_.LoadFromFile(RESSOURCES_SENSOR_OFF))
    return EXIT_FAILURE;
  for (int i = 0; i < 7; ++i)
  {
    spr_sensors_[i].SetImage(img_sensor_off_);
    spr_sensors_[i].Move(SENSOR_TAB_X + i * SENSOR_TAB_X, SENSOR_TAB_Y);
  }

  // main loop
  while (!stop_)
  {
    float dt = clock.GetElapsedTime();
    clock.Reset();

    handle_events_();
    refresh_screen_();

    // Actualise the robot.
    robot_frame(TIME_SLICE);
    update_world_(dt);

    // Quick sleep not to burn the CPU...
    sf::Sleep(0.001f);
  }

  return EXIT_SUCCESS;
}

// *************** PRIVATE METHODS ****************

void Emulator::refresh_screen_(void)
{
  // Refresh the screen.
  window_.Clear();

  // Update the map and tracer sprites given they related images could
  // have been modified.
  spr_map_.SetImage(img_map_);
  spr_tracer_.SetImage(img_tracer_);

  // Update the text for the motors
  std::ostringstream r_stream;
  std::ostringstream l_stream;
  r_stream << "right motor : " << r_motor_real_;
  l_stream << "left motor : " << l_motor_real_;
  str_r_motor_.SetText(r_stream.str());
  str_l_motor_.SetText(l_stream.str());

  // Draw everything.
  window_.Draw(spr_map_);
  if (tracer_)
    window_.Draw(spr_tracer_);
  window_.Draw(spr_robot_);
  for (int i = 0; i < 7; ++i)
    window_.Draw(spr_sensors_[i]);
  window_.Draw(str_r_motor_);
  window_.Draw(str_l_motor_);
  window_.Draw(str_sensors_);
  window_.Draw(str_legend_);

  // Generate the screen.
  window_.Display();
}

void Emulator::handle_events_(void)
{
  sf::Event event;

  // Handle events.
  while (window_.GetEvent(event))
  {
    if (event.Type == sf::Event::Closed)
    {
      stop_ = true;
      break;
    }

    if (event.Type == sf::Event::KeyPressed)
    {
      if (event.Key.Code == sf::Key::Space)
      {
        img_tracer_.Create(MAP_HEIGHT, MAP_WIDTH, sf::Color(0, 0, 0, 0));
        tracer_ = !tracer_;
      }
      if (event.Key.Code == sf::Key::F12)
      {
        sf::Image prt = window_.Capture();
        std::ostringstream name;
        name << PRT_SCREEN << prt_screen_count_ << ".jpg";
        prt.SaveToFile(name.str());
        prt_screen_count_++;
      }
    }
  }

  // Check realtime game inputs.
  const sf::Input& Input = window_.GetInput();

  // Close if escape is pressed.
  if (Input.IsKeyDown(sf::Key::Escape))
    stop_ = true;

  if (Input.IsKeyDown(sf::Key::Left))
    spr_robot_.Rotate(1);
  if (Input.IsKeyDown(sf::Key::Right))
    spr_robot_.Rotate(-1);

  if (Input.IsMouseButtonDown(sf::Mouse::Left))
  {
    // Left click set the line on the pixel over the mouse.
    int x = Input.GetMouseX() - DRAW_WIDE / 2;
    int y = Input.GetMouseY() - DRAW_WIDE / 2;
    for (int i = 0; i < DRAW_WIDE; ++i)
      for (int j = 0; j < DRAW_WIDE; ++j)
        img_map_.SetPixel(x + i, y + j, sf::Color(0, 0, 0, 255));
  }

  if (Input.IsMouseButtonDown(sf::Mouse::Right))
  {
    // Right click set the robot somewhere on the map.
    spr_robot_.SetX(Input.GetMouseX());
    spr_robot_.SetY(Input.GetMouseY());
  }
}

void Emulator::update_world_(float dt)
{
  // Update the elapsed time on the motor.
  // To avoid realtime troubles, we consider each turn is TIME_SLICE long
  r_motor_clock_ += TIME_SLICE;
  l_motor_clock_ += TIME_SLICE;

  // Use the calculated force to determine the move of the robot.
  float vect = 0;
  float rot = 0;
  if (l_motor_real_ < r_motor_real_)
  {
    vect = l_motor_real_;
    rot = r_motor_real_ - l_motor_real_;
  }
  else
  {
    vect = r_motor_real_;
    rot = r_motor_real_ - l_motor_real_;
  }

  // From the speed vector and the current angle of the robot, get the move.
  spr_robot_.Move(vect * cos(spr_robot_.GetRotation() * PI / 180) * dt,
                  - vect * sin(spr_robot_.GetRotation() * PI / 180) * dt);

  // Determine the rotation of the robot according to it radial speed
  // and how large it is.
  spr_robot_.Rotate(atan(rot / ROBOT_WIDTH) * dt * ROBOT_ROT_SPEED);

  // Set the current state for each sensor.
  sf::Vector2f local_pos_sensor(img_robot_.GetWidth(), 0);
  sf::Vector2f global_pos_sensor(0, 0);
  local_pos_sensor.y = img_robot_.GetHeight() / 2 - 4 * ROBOT_SENSOR_WIDTH;
  for (int i = 0; i < 7; ++i)
  {
    local_pos_sensor.y += ROBOT_SENSOR_WIDTH;
    global_pos_sensor = spr_robot_.TransformToGlobal(local_pos_sensor);
    if (global_pos_sensor.x > 0 && global_pos_sensor.x < img_map_.GetWidth() &&
                                                         global_pos_sensor.y > 0 && global_pos_sensor.y < img_map_.GetHeight())
    {
      if (img_map_.GetPixel(global_pos_sensor.x,
                            global_pos_sensor.y) == sf::Color(0, 0, 0, 255))
      {
        robot_sensors_[i] = 1;
        spr_sensors_[i].SetImage(img_sensor_on_);
      }
      else
      {
        robot_sensors_[i] = 0;
        spr_sensors_[i].SetImage(img_sensor_off_);
      }
      if (tracer_)
      {
        if (robot_sensors_[i] == 1)
          img_tracer_.SetPixel(global_pos_sensor.x, global_pos_sensor.y,
                               sf::Color(25, 125, 125, 255));
        else
          img_tracer_.SetPixel(global_pos_sensor.x, global_pos_sensor.y,
                               sf::Color(200, 100, 100, 255));
      }
    }
}
  // Update the current positions of the motors.
  for (int i = 0; i < 4; ++i)
  {
    r_motor_prev_[i] = r_motor_[i];
    l_motor_prev_[i] = l_motor_[i];
  }
}

// ************ Friend functions **************

static float compute_speed(int *mot_curr, int *mot_prev, float mot_dt)
{
  Emulator *em = Emulator::get_instance();

  // If more than one magnet is enable at a time,
  // we consider the motor is jammed.
  if (mot_curr[0] + mot_curr[1] + mot_curr[2] + mot_curr[3] > 1)
    return 0;

  // Find the active magnet.
  for (int i = 0; i < 4; ++i)
    if (mot_curr[i] == 1)
    {
      if (mot_prev[(i + 1) % 4] == 1)
      {
        // The next magnet was previously enabled. Thus speed is negative.
	return - (1 / mot_dt);
      }
      if (mot_prev[(i + 3) % 4] == 1)
      {
        // The previous magnet was previously enabled. Thus speed is positive.
	return (1 / mot_dt);
      }
      break;
    }
}

int set_r_motor(int *motor)
{
  Emulator *em = Emulator::get_instance();

  // Determine the motor speed.
  em->r_motor_real_ = compute_speed(motor, em->r_motor_, em->r_motor_clock_);
  em->r_motor_clock_ = 0;

  // Save the new state of the motor.
  for (int i = 0; i < 4; ++i)
    em->r_motor_[i] = motor[i];

  return 0;
}

int set_l_motor(int *motor)
{
  Emulator *em = Emulator::get_instance();

  // Determine the motor speed.
  em->l_motor_real_ = compute_speed(motor, em->l_motor_, em->l_motor_clock_);
  em->l_motor_clock_ = 0;

  // Save the new state of the motor.
  for (int i = 0; i < 4; ++i)
    em->l_motor_[i] = motor[i];

  return 0;
}

int set_leds(int *leds)
{
  Emulator *em = Emulator::get_instance();

  for (int i = 0; i < 7; ++i)
  {
    em->robot_leds_[i] = leds[i];
  }

  return 0;
}

int get_sensors(int *sensors)
{
  Emulator *em = Emulator::get_instance();

  for (int i = 0; i < 7; ++i)
    sensors[i] = em->robot_sensors_[i] & em->robot_leds_[i];

  return 0;
}
