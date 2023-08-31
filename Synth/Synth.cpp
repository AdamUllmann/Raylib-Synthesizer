// Synth.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "raylib.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include <string>

const int sampleRate = 44100;
const float twoPi = 2 * PI;
const int defaultBufferSize = 1024;

float tuningFactor = 1.0f;

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
    float value = phase / PI;
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

float phaseWave(float frequency, float& phase) {
    float value = phase;
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

void controls(float &frequency, bool &keyDown) {
    if (IsKeyPressed(KEY_UP)) {     // tuning
        tuningFactor *= 2;  
    }
    if (IsKeyPressed(KEY_DOWN)) {
        tuningFactor *= 0.5;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        tuningFactor *= 1.01;
    }
    if (IsKeyDown(KEY_LEFT)) {
        tuningFactor *= 0.99;
    }

    for (const auto& pair : keyToNote) {
        if (IsKeyDown(pair.first)) {
            frequency = noteFrequencies[pair.second] * tuningFactor;
            keyDown = true;
            break;
        }
    }
}


const int oscilloscopeSize = 800;
short circularBuffer[oscilloscopeSize];
int bufferIndex = 0;

int main() {  
    InitWindow(800, 550, "synth"); 
    InitAudioDevice();
    
    SetAudioStreamBufferSizeDefault(defaultBufferSize);

    AudioStream stream = LoadAudioStream(sampleRate, 16, 1);
    PlayAudioStream(stream);

    float frequency = 220.0f;
    float phase = 0.0f;

    std::string wave = "sine";
    bool keyDown = false;

    std::vector<std::string> waveTypes = { "sine", "saw", "square", "triangle", "phase", "noise"};
    int currentWaveTypeIndex = 0;

    std::vector<short> buffer(defaultBufferSize);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {

        if (IsAudioStreamProcessed(stream)) {
            if (keyDown == true) {
                for (int i = 0; i < defaultBufferSize; i++) {
                    if (waveTypes[currentWaveTypeIndex] == "sine") {
                        buffer[i] = static_cast<short>(sineWave(frequency, phase) * INT16_MAX);
                    }
                    else if (waveTypes[currentWaveTypeIndex] == "saw") {
                        buffer[i] = static_cast<short>(sawWave(frequency, phase) * INT16_MAX);
                    }
                    else if (waveTypes[currentWaveTypeIndex] == "square") {
                        buffer[i] = static_cast<short>(squareWave(frequency, phase) * INT16_MAX);
                    }
                    else if (waveTypes[currentWaveTypeIndex] == "triangle") {
                        buffer[i] = static_cast<short> (triangleWave(frequency, phase) * INT16_MAX);
                    }
                    else if (waveTypes[currentWaveTypeIndex] == "phase") {
                        buffer[i] = static_cast<short> (phaseWave(frequency, phase) * INT16_MAX);
                    }
                    else if (waveTypes[currentWaveTypeIndex] == "noise") {
                        buffer[i] = static_cast<short> (noise(frequency, phase) * INT16_MAX);
                    }


                    circularBuffer[bufferIndex] = buffer[i];
                    bufferIndex = (bufferIndex + 1) % oscilloscopeSize;
                }
                UpdateAudioStream(stream, buffer.data(), defaultBufferSize);
            }
        }
        keyDown = false;
        controls(frequency, keyDown);

        if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
            currentWaveTypeIndex = (currentWaveTypeIndex + 1) % waveTypes.size();
        }




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

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

