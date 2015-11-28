#!/bin/sh

cc -o qmap src/*.c `sdl2-config --cflags --libs` -lm -pedantic -Wall
