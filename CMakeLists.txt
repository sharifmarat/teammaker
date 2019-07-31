cmake_minimum_required (VERSION 2.6)
project (teammaker)
add_executable(teammaker
  src/main.cpp
  src/strings.h)
set_property(TARGET teammaker PROPERTY CXX_STANDARD 14)
