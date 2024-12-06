#include "Oscillator.h"
#include "Globals.h"


namespace Oscillator {
    float generateSample(float frequency, float& phase) {
        return sineWave(frequency, phase);
    }

    void updatePhase(float frequency, float& phase) {
        phase += twoPi * frequency / sampleRate;
        if (phase >= twoPi) {
            phase -= twoPi;
        }
    }

    // Wave function definitions
    float sineWave(float frequency, float& phase) {
        float value = sinf(phase);
        updatePhase(frequency, phase);
        return value;
    }
    float sawWave(float frequency, float& phase) {
        float value = 2.0 * (phase / twoPi) - 1.0;
        updatePhase(frequency, phase);
        return value;
    }
    float squareWave(float frequency, float& phase) {
        float value;
        if (phase < PI)     value = 1.0f;
        else                value = -1.0f;
        updatePhase(frequency, phase);
        return value;
    }
    float triangleWave(float frequency, float& phase) {
        float value;
        if (phase < PI)     value = -1.0f + (2 / PI) * phase;
        else                value = 3.0f - (2 / PI) * phase;
        updatePhase(frequency, phase);
        return value;
    }
    float noise(float frequency, float& phase) {
        float value = (float)GetRandomValue(-1, 1);
        updatePhase(frequency, phase);
        return value;
    }
    /*void resetPhasesForActiveFrequencies() {      // useful for debugging
        for (float freq : activeFrequencies) {
            phases[freq] = 0.0f;
        }
    } */
}
