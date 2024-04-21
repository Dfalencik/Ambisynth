#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>


class SynthVoice {
public:
    SynthVoice();
    ~SynthVoice() = default;

    void initializeWavetable();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void startNote(int midiNoteNumber, float velocity);
    void stopNote(bool allowTailOff);
    int getNoteNumber() const;  // Ensure declaration is correct
    float getNextSample();
    bool isActive() const;

    void setUnisonSize(int size);
    void setDetuneAmount(float detune);
    void setWavetable(const std::array<std::vector<float>, 4>& newWavetable);
private:
    float midiNoteToFrequency(int midiNoteNumber) const;
    float getSampleRate() const;

    bool active;
    float frequency;
    int midiNoteNumber;
    double currentSampleIndex;
    double increment;
    float velocity;
    float amplitude;
    int currentWaveform;
    std::array<std::vector<float>, 4> wavetable;
    static constexpr size_t wavetableSize = 2048;
    std::vector<float> detuneOffsets;
    int unisonSize;
    float detuneAmount;

    void calculateDetuneOffsets();
};
