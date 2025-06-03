# Calculator
A fully functional calculator application built with C and GTK3, featuring a modern dark theme and comprehensive functionality.

sudo apt update
sudo apt install build-essential
sudo apt install libgtk-3-dev
sudo apt install pkg-config

gcc calculator.c -o calculator `pkg-config --cflags --libs gtk+-3.0` -lm

# Install development packages
sudo apt install libgtk-*

