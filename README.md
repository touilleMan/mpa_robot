# MPA_robot

Car-like robot simulator for testing moving algorythms 

# Overview

This 2D top-view simulator is aimed at testing moving algorythm on line tracer robot.

# Feature

This software is divided in two parts.

### The simulator

The simulator is written in C++ with the sfml graphical library.
The simulator create simple a 2D top-view world with the possibility to draw lines on the ground.

### The algorythm

A C algorythm :
 - receive in input the current view of the line sensors
 - set as output the motors' polarity to make the robot move
