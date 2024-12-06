#include "Filter.h"
#include <raymath.h>

Biquad::Biquad() : a0(0), a1(0), a2(0), b0(1), b1(0), b2(0), x1(0), x2(0), y1(0), y2(0) {}

void Biquad::setLowPass(float sampleRate, float cutoff, float Q) {
    float w0 = 2.0f * PI * cutoff / sampleRate;
    float alpha = sin(w0) / (2.0f * Q);
    float cosw0 = cos(w0);

    b0 = (1.0f - cosw0) / 2.0f;
    b1 = 1.0f - cosw0;
    b2 = b0;
    a0 = 1.0f + alpha;
    a1 = -2.0f * cosw0;
    a2 = 1.0f - alpha;

    b0 /= a0;   // normalize
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}

float Biquad::process(float in) {
    float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

    x2 = x1;
    x1 = in;

    y2 = y1;
    y1 = out;

    return out;
}
