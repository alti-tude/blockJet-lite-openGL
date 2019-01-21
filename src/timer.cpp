#include "timer.h"
#include <GLFW/glfw3.h>

Timer::Timer(double interval) {
    prev = 0;
    frameCount = 0;
    this->interval = interval;
}

bool Timer::processTick() {
    double cur = glfwGetTime();
    if (cur - prev >= interval) {
        prev = cur;
        frameCount++;
        return true;
    } else
        return false;
}

