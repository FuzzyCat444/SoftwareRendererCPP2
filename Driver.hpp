#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <iostream>

#include <SFML/Graphics.hpp>

#include "Renderer.hpp"

class Driver
{
public:
    Driver();
    Driver(int width, int height, int scale);

    void start();
private:
    int width;
    int height;
    int scale;
    int windowWidth;
    int windowHeight;
};

#endif
