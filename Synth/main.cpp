#include "Synth.h"
#include "Controls.h"
#include "Globals.h"


int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 550, "synth");
    InitAudioDevice();

    SetAudioStreamBufferSizeDefault(defaultBufferSize);

    AudioStream stream = LoadAudioStream(sampleRate, 16, 1);
    PlayAudioStream(stream);

    float frequency = 440.0f;
    filter.setLowPass(sampleRate, cutoff, resonance);

    std::vector<short> buffer(defaultBufferSize);

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsAudioStreamProcessed(stream)) {
            generateAudioBuffer(buffer);
            UpdateAudioStream(stream, buffer.data(), defaultBufferSize);
        }
        controls();

        BeginDrawing();
        ClearBackground(BLACK);
        drawOscilloscope();
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
