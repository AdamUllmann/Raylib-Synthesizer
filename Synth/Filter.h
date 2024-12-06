#pragma once
#ifndef FILTER_H
#define FILTER_H

class Biquad {
private:
    float a0, a1, a2, b0, b1, b2; // Coefficients
    float x1, x2, y1, y2;         // Input/output history

public:
    Biquad();
    void setLowPass(float sampleRate, float cutoff, float Q);
    float process(float in);
};

#endif
