#pragma once
#ifndef SYNTH_H
#define SYNTH_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include "raylib.h"
#include "Filter.h"
#include "Oscillator.h"

void generateAudioBuffer(std::vector<short>& buffer);
void drawOscilloscope();

#endif
