// Synth.cpp
// By Adam Ullmann

#include "raylib.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include <iostream>

const int sampleRate = 44100;
const float twoPi = 2 * PI;
const int defaultBufferSize = 1024;

float tuningFactor = 1.0f;
std::set<float> activeFrequencies;
std::unordered_map<float, float> phases;

// UI elements
Rectangle cycleWaveformButton = { 10, 10, 150, 40 };
Rectangle octaveUpButton = { 10, 60, 150, 40 };
Rectangle octaveDownButton = { 10, 110, 150, 40 };
Rectangle tuneKnob = { 10, 160, 40, 40 };

// filter
float cutoff = 1000.0f; 
float resonance = 2.0f;

class StateVariableFilter {
private:
    float fs; // sample rate
    float f;  // frequency
    float q;  // resonance

    float low, band, high; // filter states

public:
    StateVariableFilter(float sampleRate) : fs(sampleRate), f(440.0), q(1.0), low(0), band(0), high(0) {}

    void setCutoff(float cutoff) {
        if (cutoff > 5500.0f) cutoff = 5500.0f;
        if (cutoff < 0.0f) cutoff = 0.0f;
        f = cutoff;
    }
    void setResonance(float resonance) { q = resonance; }

    float process(float input) {
        float F = 2.0 * sin((PI * f) / fs);
        low += F * band;
        high = input - low - q * band;
        band += F * high;
        return low; // returns low-pass. can be changed to band for band-pass and high for high-pass
    }
};



std::unordered_map<std::string, float> noteFrequencies = {
    // octave 3
    {"C3", 130.81f},
    {"C#3", 138.59f},
    {"D3", 146.83f},
    {"D#3", 155.56f},
    {"E3", 164.81f},
    {"F3", 174.61f},
    {"F#3", 185.00f},
    {"G3", 196.00f},
    {"G#3", 207.65f},
    {"A3", 220.00f},
    {"A#3", 233.08f},
    {"B3", 246.94f},

    // octave 4
    {"C4", 261.63f},
    {"C#4", 277.18f},
    {"D4", 293.66f},
    {"D#4", 311.13f},
    {"E4", 329.63f},
    {"F4", 349.23f},
    {"F#4", 369.99f},
    {"G4", 392.00f},
    {"G#4", 415.30f},
    {"A4", 440.00f},
    {"A#4", 466.16f},
    {"B4", 493.88f},

    // octave 5
    {"C5", 523.25f},
    {"C#5", 554.37f},
    {"D5", 587.33f},
    {"D#5", 622.25f},
    {"E5", 659.26f},
    {"F5", 698.46f},
    {"F#5", 739.99f},
    {"G5", 783.99f},
    {"G#5", 830.61f},
    {"A5", 880.00f},
    {"A#5", 932.33f},
    {"B5", 987.77f}

};

std::unordered_map<int, std::string> keyToNote = {
    // white keys starting from C3
    {KEY_Z, "C3"}, {KEY_X, "D3"}, {KEY_C, "E3"}, {KEY_V, "F3"}, {KEY_B, "G3"},
    {KEY_N, "A3"}, {KEY_M, "B3"}, {KEY_COMMA, "C4"}, {KEY_PERIOD, "D4"}, {KEY_SLASH, "E4"},

    // black keys starting from C#3
    {KEY_S, "C#3"}, {KEY_D, "D#3"}, {KEY_G, "F#3"}, {KEY_H, "G#3"}, {KEY_J, "A#3"},
    {KEY_L, "C#4"}, {KEY_SEMICOLON, "D#4"},

    // white keys starting from C4
    {KEY_Q, "C4"}, {KEY_W, "D4"}, {KEY_E, "E4"}, {KEY_R, "F4"}, {KEY_T, "G4"},
    {KEY_Y, "A4"}, {KEY_U, "B4"}, {KEY_I, "C5"}, {KEY_O, "D5"}, {KEY_P, "E5"},
    {KEY_LEFT_BRACKET, "F5"}, {KEY_RIGHT_BRACKET, "G5"},

    // black keys starting from C#4
    {KEY_TWO, "C#4"}, {KEY_THREE, "D#4"}, {KEY_FIVE, "F#4"}, {KEY_SIX, "G#4"},
    {KEY_SEVEN, "A#4"}, {KEY_NINE, "C#5"}, {KEY_ZERO, "D#5"}, {KEY_EQUAL, "F#5"}
};

float sineWave(float frequency, float& phase) {
    float value = sinf(phase);
    phase += twoPi * frequency / sampleRate;
    if (phase >= twoPi) { 
        phase -= twoPi; 
    }
    return value;
}

float sawWave(float frequency, float& phase) {
    float value = 2.0 * (phase / twoPi) - 1.0;
    phase += twoPi * frequency / sampleRate;
    if (phase >= twoPi) {
        phase -= twoPi;
    }
    return value;
}

float squareWave(float frequency, float& phase) {
    float value;
    if (phase < PI) {
        value = 1.0f;
    }
    else {
        value = -1.0f;
    }
    phase += twoPi * frequency / sampleRate;
    if (phase >= twoPi) {
        phase -= twoPi;
    }
    return value;
}

float triangleWave(float frequency, float& phase) {
    float value;
    if (phase < PI) {
        value = -1.0f + (2 / PI) * phase;
    }
    else {
        value = 3.0f - (2 / PI) * phase;
    }
    phase += twoPi * frequency / sampleRate;
    if (phase >= twoPi) {
        phase -= twoPi;
    }
    return value;
}

float noise(float frequency, float& phase) {
    float value = (float)GetRandomValue(-1, 1);
    phase += twoPi * frequency / sampleRate;
    if (phase >= twoPi) {
        phase -= twoPi;
    }
    return value;
}

/*void resetPhasesForActiveFrequencies() {
    for (float freq : activeFrequencies) {
        phases[freq] = 0.0f;
    }
} */

void controls() {

    if (IsKeyPressed(KEY_UP)) {     // tuning
        tuningFactor *= 2;
        //resetPhasesForActiveFrequencies();
    }
    if (IsKeyPressed(KEY_DOWN)) {
        tuningFactor *= 0.5;
        //resetPhasesForActiveFrequencies();
    }
    if (IsKeyDown(KEY_RIGHT)) {
        tuningFactor *= 1.01;
        //resetPhasesForActiveFrequencies();
    }
    if (IsKeyDown(KEY_LEFT)) {
        tuningFactor *= 0.99;
        //resetPhasesForActiveFrequencies();
    }

    for (const auto& pair : keyToNote) {
        float tunedFreq = noteFrequencies[pair.second];// * tuningFactor;
        if (IsKeyDown(pair.first) && activeFrequencies.find(tunedFreq) == activeFrequencies.end()) {
            activeFrequencies.insert(tunedFreq);
        }
        else if (IsKeyReleased(pair.first)) {
            activeFrequencies.erase(tunedFreq);
        }
    }
}

const int oscilloscopeSize = 800;
short circularBuffer[oscilloscopeSize];
int bufferIndex = 0;

int main() {  
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 550, "synth"); 
    InitAudioDevice();
    
    SetAudioStreamBufferSizeDefault(defaultBufferSize);

    AudioStream stream = LoadAudioStream(sampleRate, 16, 1);
    PlayAudioStream(stream);

    float frequency = 440.0f;
    float phase = 0.0f;

    StateVariableFilter svf(sampleRate);
    svf.setCutoff(1000.0);
    svf.setResonance(2.0);
    StateVariableFilter svf1(sampleRate);   // for 4-pole
    svf1.setCutoff(1000.0);
    svf1.setResonance(2.0);
    StateVariableFilter svf2(sampleRate);   // for 6-pole
    svf2.setCutoff(1000.0);
    svf2.setResonance(2.0);

    std::string wave = "sine";

    std::vector<std::string> waveTypes = { "sine", "saw", "square", "triangle", "noise"};
    int currentWaveTypeIndex = 0;

    std::vector<short> buffer(defaultBufferSize);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {

        if (IsAudioStreamProcessed(stream)) {
            if (activeFrequencies.size() > 0) {
                for (int i = 0; i < defaultBufferSize; i++) {
                    float sampleValue = 0.0f;
                    for (float freq : activeFrequencies) {
                        float tunedFreq = freq * tuningFactor;
                        double phaseIncrement = twoPi * tunedFreq / sampleRate;
                        if (waveTypes[currentWaveTypeIndex] == "sine") {
                            sampleValue += sineWave(tunedFreq, phases[freq]);
                        }
                        else if (waveTypes[currentWaveTypeIndex] == "saw") {
                            sampleValue += sawWave(tunedFreq, phases[freq]);
                        }
                        else if (waveTypes[currentWaveTypeIndex] == "square") {
                            sampleValue += squareWave(tunedFreq, phases[freq]);
                        }
                        else if (waveTypes[currentWaveTypeIndex] == "triangle") {
                            sampleValue += triangleWave(tunedFreq, phases[freq]);
                        }
                        else if (waveTypes[currentWaveTypeIndex] == "noise") {
                            sampleValue += noise(tunedFreq, phases[freq]);
                        }
                        
                        phases[freq] += phaseIncrement;
                        if (phases[freq] > twoPi) {
                            phases[freq] -= twoPi;
                        }
                    }
                    sampleValue /= activeFrequencies.size() > 0 ? activeFrequencies.size() : 1;

                    sampleValue = svf.process(sampleValue);

                     sampleValue = svf1.process(sampleValue);
                     //sampleValue = svf2.process(sampleValue);

                    sampleValue *= 0.5;
                    buffer[i] = static_cast<short>(sampleValue * INT16_MAX);

                    circularBuffer[bufferIndex] = buffer[i];
                    bufferIndex = (bufferIndex + 1) % oscilloscopeSize;
                }
                UpdateAudioStream(stream, buffer.data(), defaultBufferSize);
            }
        }
        controls();

        if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
            currentWaveTypeIndex = (currentWaveTypeIndex + 1) % waveTypes.size();
        }
        if (IsKeyDown(KEY_GRAVE)) {
            cutoff -= 100.0;
            svf.setCutoff(cutoff);
            svf1.setCutoff(cutoff);
            svf2.setCutoff(cutoff);
            std::cout << cutoff << std::endl;
        }
        if (IsKeyDown(KEY_ONE)) {
            cutoff += 100.0;
            svf.setCutoff(cutoff);
            svf1.setCutoff(cutoff);
            svf2.setCutoff(cutoff);
            std::cout << cutoff << std::endl;
        }

        /*if (CheckCollisionPointRec(GetMousePosition(), cycleWaveformButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentWaveTypeIndex = (currentWaveTypeIndex + 1) % waveTypes.size();
        }
        if (CheckCollisionPointRec(GetMousePosition(), octaveUpButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            tuningFactor *= 2;
        }
        if (CheckCollisionPointRec(GetMousePosition(), octaveDownButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            tuningFactor *= 0.5;
        }
        if (CheckCollisionPointRec(GetMousePosition(), tuneKnob) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            tuningFactor += (GetMouseX() - (tuneKnob.x + tuneKnob.width / 2.0f)) * 0.01f;
        } */


        BeginDrawing();
        ClearBackground(RAYWHITE);

       for (int i = 1; i < oscilloscopeSize; i++) {
            int prevIndex = (bufferIndex + i - 1) % oscilloscopeSize;
            int currentIndex = (bufferIndex + i) % oscilloscopeSize;

            float prevSample = circularBuffer[prevIndex] / (float)INT16_MAX;
            float currentSample = circularBuffer[currentIndex] / (float)INT16_MAX;

            float prevY = (1.0f - prevSample) * GetScreenHeight() / 2.0f;
            float currentY = (1.0f - currentSample) * GetScreenHeight() / 2.0f;

            DrawLineEx({ float(i - 1), prevY }, { float(i), currentY }, 5.0, BLUE);
        }

      /* DrawRectangleRec(cycleWaveformButton, LIGHTGRAY);
       DrawText("Cycle Waveform", cycleWaveformButton.x + 5, cycleWaveformButton.y + 10, 20, DARKGRAY);

       DrawRectangleRec(octaveUpButton, LIGHTGRAY);
       DrawText("Octave Up", octaveUpButton.x + 5, octaveUpButton.y + 10, 20, DARKGRAY);

       DrawRectangleRec(octaveDownButton, LIGHTGRAY);
       DrawText("Octave Down", octaveDownButton.x + 5, octaveDownButton.y + 10, 20, DARKGRAY);

       DrawRectangleRec(tuneKnob, LIGHTGRAY);
       DrawText("Tune", tuneKnob.x + 5, tuneKnob.y - 20, 20, DARKGRAY); */

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

