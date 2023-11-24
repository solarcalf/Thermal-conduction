cmake_minimum_required(VERSION 2.8)

project(ThermalConduction)

add_executable(ThermalConduction main.cpp ThermalConduction.h TMA.h)
set_target_properties(ThermalConduction PROPERTIES RUNTIME_OUTPUT_DIRECTORY "Interface")