#pragma once
#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cmath>

namespace Oscillator {
    float generateSample(float frequency, float& phase);
    float sineWave(float frequency, float& phase);
    float sawWave(float frequency, float& phase);
    float squareWave(float frequency, float& phase);
    float triangleWave(float frequency, float& phase);
    float noise(float frequency, float& phase);
};

#endif
