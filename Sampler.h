#pragma once
#include <memory>
#include <JuceHeader.h>

class Sampler {
public:
    Sampler();
    ~Sampler();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    void loadSample(const juce::String& path);
    void setVolume(float newVolume);
    void handleNoteOn(int midiNoteNumber, float velocity);
    void handleNoteOff(int midiNoteNumber, float velocity);
    void renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples);

    std::unique_ptr<juce::AudioFormatReader> formatReader;  // Handles reading audio files

private:
    float volume = 1.0f;  // Default volume level
    juce::Synthesiser synth;  // Synthesizer to manage sample playback
    juce::MidiMessageCollector midiCollector;  // Collects and time-stamps incoming MIDI messages
    juce::AudioFormatManager formatManager;  // Manages audio formats

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sampler)  // Detect memory leaks and copying issues
};
