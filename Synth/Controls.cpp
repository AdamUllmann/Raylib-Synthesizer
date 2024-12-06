#include "Controls.h"
#include "Synth.h"
#include "raylib.h"
#include "Globals.h"


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
        float tunedFreq = noteToFrequency[pair.second];   // * tuningFactor;
        if (IsKeyDown(pair.first) && activeFrequencies.find(tunedFreq) == activeFrequencies.end()) {
            activeFrequencies.insert(tunedFreq);
        }
        else if (IsKeyReleased(pair.first)) {
            activeFrequencies.erase(tunedFreq);
        }
    }

    const float incrementFactor = 0.05;
    if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
        currentWaveTypeIndex = (currentWaveTypeIndex + 1) % waveTypes.size();
    }
    if (IsKeyDown(KEY_GRAVE)) {
        cutoff -= cutoff * incrementFactor;
        if (cutoff < 0.5f) cutoff = 0.5f;                   // we do 0.5 because of the cutoff will never reach 0
        filter.setLowPass(sampleRate, cutoff, resonance);
    }
    if (IsKeyDown(KEY_ONE)) {
        cutoff += cutoff * incrementFactor;
        if (cutoff > 21000.0f) cutoff = 21000.0f;
        filter.setLowPass(sampleRate, cutoff, resonance);
    }
    if (IsKeyDown(KEY_BACKSLASH)) {
        resonance -= 0.1f;
        if (resonance < 0.707f) resonance = 0.707f;         // picked this arbitrarily. its the number you would use to get RMS voltage (and it's used in analog filters).
        filter.setLowPass(sampleRate, cutoff, resonance);                                                                              // (it means nothing here)
    }
    if (IsKeyDown(KEY_BACKSPACE)) {
        resonance += 0.1f;
        if (resonance > 100.0f) resonance = 100.0f;
        filter.setLowPass(sampleRate, cutoff, resonance);
    }
}

