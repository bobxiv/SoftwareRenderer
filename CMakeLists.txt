cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/libpng" CACHE STRING "Where is LibPNG installed?")

##################################################################
#############               SOLUTION                 #############
##################################################################

project(SoftwareRenderer LANGUAGES CXX)
# Version
set (softwareRenderer_VERSION_MAJOR 1)
set (softwareRenderer_VERSION_MINOR 0)

##################################################################
#############       SOFTWARE RENDERER PROJECT        #############
##################################################################

add_subdirectory(src/)