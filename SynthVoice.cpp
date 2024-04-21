#include "SynthVoice.h"
#include <cmath>
#include <algorithm>

SynthVoice::SynthVoice() : active(false), frequency(440.0f), midiNoteNumber(-1),
                           currentSampleIndex(0.0), increment(0.0), velocity(0.0),
                           amplitude(1.0), currentWaveform(0), unisonSize(1), detuneAmount(0.0f) {
    initializeWavetable();
}

void SynthVoice::initializeWavetable() {
    for (auto& table : wavetable) {
        table.resize(wavetableSize);
        for (size_t i = 0; i < wavetableSize; ++i) {
            table[i] = std::sin(2.0 * M_PI * i / wavetableSize);
        }
    }
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Sample rate dependent calculations here
}

void SynthVoice::setWavetable(const std::array<std::vector<float>, 4>& newWavetable) {
    wavetable = newWavetable;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity) {
    this->midiNoteNumber = midiNoteNumber;
    this->velocity = std::clamp(velocity, 0.0f, 1.0f);
    this->frequency = midiNoteToFrequency(midiNoteNumber);
    this->increment = (frequency / std::max(getSampleRate(), 1.0f)) * wavetableSize;
    this->currentSampleIndex = 0;
    this->active = true;
}

void SynthVoice::stopNote(bool allowTailOff) {
    active = false;
}

int SynthVoice::getNoteNumber() const {
    return midiNoteNumber;
}

float SynthVoice::getNextSample() {
    float sample = 0.0f;
    for (int i = 0; i < unisonSize; ++i) {
        int index = static_cast<int>(currentSampleIndex * detuneOffsets[i]) % wavetableSize;
        sample += wavetable[currentWaveform][index];
        currentSampleIndex += increment;
        if (currentSampleIndex >= wavetableSize) currentSampleIndex -= wavetableSize;
    }
    return sample / unisonSize;
}

bool SynthVoice::isActive() const {
    return active;
}

void SynthVoice::setUnisonSize(int size) {
    unisonSize = size;
    calculateDetuneOffsets();
}

void SynthVoice::setDetuneAmount(float detune) {
    detuneAmount = detune;
    calculateDetuneOffsets();
}

void SynthVoice::calculateDetuneOffsets() {
    detuneOffsets.resize(unisonSize);
    for (int i = 0; i < unisonSize; ++i) {
        float offset = (i - unisonSize / 2) * detuneAmount;
        detuneOffsets[i] = pow(2.0f, offset / 12.0f);
    }
}

float SynthVoice::midiNoteToFrequency(int midiNoteNumber) const {
    return 440.0 * pow(2.0, (midiNoteNumber - 69) / 12.0);
}

float SynthVoice::getSampleRate() const {
    return 48000.0f;  // Replace with the actual sample rate from your audio engine context
}
