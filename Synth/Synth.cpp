#include "Synth.h"
#include "Globals.h"

const int oscilloscopeSize = 800;
short circularBuffer[oscilloscopeSize];
int bufferIndex = 0;

void generateAudioBuffer(std::vector<short>& buffer) {
    if (activeFrequencies.empty()) {
        std::fill(buffer.begin(), buffer.end(), 0);
        return;
    }

    for (int i = 0; i < defaultBufferSize; i++) {
        float sampleValue = 0.0f;
        for (float freq : activeFrequencies) {
            float tunedFreq = freq * tuningFactor;
            float phaseIncrement = twoPi * tunedFreq / sampleRate;
            if (waveTypes[currentWaveTypeIndex] == "sine") {
                sampleValue += Oscillator::sineWave(tunedFreq, phases[freq]);
            }
            else if (waveTypes[currentWaveTypeIndex] == "saw") {
                sampleValue += Oscillator::sawWave(tunedFreq, phases[freq]);
            }
            else if (waveTypes[currentWaveTypeIndex] == "square") {
                sampleValue += Oscillator::squareWave(tunedFreq, phases[freq]);
            }
            else if (waveTypes[currentWaveTypeIndex] == "triangle") {
                sampleValue += Oscillator::triangleWave(tunedFreq, phases[freq]);
            }
            else if (waveTypes[currentWaveTypeIndex] == "noise") {
                sampleValue += Oscillator::noise(tunedFreq, phases[freq]);
            }

            phases[freq] += phaseIncrement;
            if (phases[freq] > twoPi) {
                phases[freq] -= twoPi;
            }
        }
        sampleValue /= activeFrequencies.size() > 0 ? activeFrequencies.size() : 1;

        sampleValue = filter.process(sampleValue);

        sampleValue *= 0.5;
        if (sampleValue > 1.0f) sampleValue = 1.0f;
        if (sampleValue < -1.0f) sampleValue = -1.0f;

        buffer[i] = static_cast<short>(sampleValue * INT16_MAX);

        circularBuffer[bufferIndex] = buffer[i];
        bufferIndex = (bufferIndex + 1) % oscilloscopeSize;
    }
}

void drawOscilloscope() {
    for (int i = 1; i < oscilloscopeSize; i++) {
        int prevIndex = (bufferIndex + i - 1) % oscilloscopeSize;
        int currentIndex = (bufferIndex + i) % oscilloscopeSize;

        float prevSample = circularBuffer[prevIndex] / (float)INT16_MAX;
        float currentSample = circularBuffer[currentIndex] / (float)INT16_MAX;

        float prevY = (1.0f - prevSample) * GetScreenHeight() / 2.0f;
        float currentY = (1.0f - currentSample) * GetScreenHeight() / 2.0f;

        DrawLineEx({ float(i - 1), prevY }, { float(i), currentY }, 5.0, BLUE);
    }
}
