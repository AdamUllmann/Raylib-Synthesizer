#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <set>
#include "Filter.h"

// Constants
extern const int sampleRate;
extern const float twoPi;
extern const int defaultBufferSize;

// Global Variables
extern std::set<float> activeFrequencies;
extern std::unordered_map<float, float> phases;
extern float tuningFactor;
extern std::vector<std::string> waveTypes;
extern int currentWaveTypeIndex;
extern Biquad filter;
extern float cutoff;
extern float resonance;
extern std::unordered_map<std::string, float> noteToFrequency;
extern std::unordered_map<int, std::string> keyToNote;

#endif
