// Synth.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "raylib.h"
#include <cmath>
#include <vector>

const int sampleRate = 44100;
const float twoPi = 2 * PI;
const int defaultBufferSize = 1024;

float sineWave(float frequency, float& phase) {
    float value = sinf(phase);
    phase += twoPi * frequency / sampleRate;
    if (phase >= twoPi) { 
        phase -= twoPi; 
    }
    return value;
}

int main() {
    InitWindow(400, 225, "synth");
    InitAudioDevice();

    SetAudioStreamBufferSizeDefault(defaultBufferSize);

    AudioStream stream = LoadAudioStream(sampleRate, 16, 1);
    PlayAudioStream(stream);

    float frequency = 220.0f;
    float phase = 0.0f;

    std::vector<short> buffer(defaultBufferSize);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < defaultBufferSize; i++) {
                buffer[i] = static_cast<short>(sineWave(frequency, phase) * INT16_MAX); // 32767
            }
            UpdateAudioStream(stream, buffer.data(), defaultBufferSize);
        }

        if (IsKeyPressed(KEY_UP)) {
            frequency *= 1.059; // ratio between half-steps
        }
        if (IsKeyPressed(KEY_DOWN)) {
            frequency *= 0.941;
        }
        if (IsKeyDown(KEY_W)) {
            frequency += 2;
        }
        if (IsKeyDown(KEY_S)) {
            frequency -= 2;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

