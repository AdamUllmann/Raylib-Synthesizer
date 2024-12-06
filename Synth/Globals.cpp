#include "Globals.h"

const int sampleRate = 44100;
const float twoPi = 2 * PI;
const int defaultBufferSize = 1024;

std::set<float> activeFrequencies;
std::unordered_map<float, float> phases;
float tuningFactor = 1.0f;
std::vector<std::string> waveTypes = { "sine", "saw", "square", "triangle", "noise" };
int currentWaveTypeIndex = 0;
float cutoff = 1000.0f;
float resonance = 1.0f;

Biquad filter;

std::unordered_map<std::string, float> noteToFrequency = {
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
