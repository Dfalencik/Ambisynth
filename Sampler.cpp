#include "Sampler.h"

Sampler::Sampler() {
    formatManager.registerBasicFormats();  // Register supported audio formats
}

Sampler::~Sampler() {
    // Resources are automatically cleaned up by unique_ptr and other JUCE-managed objects
}

void Sampler::prepareToPlay(double sampleRate, int samplesPerBlock) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
    DBG("Sampler prepared with Sample Rate: " << sampleRate << ", Samples Per Block: " << samplesPerBlock);
}

void Sampler::releaseResources() {
    DBG("Releasing Sampler resources.");
}

void Sampler::loadSample(const juce::String& path) {
    juce::File file(path);
    formatReader.reset(formatManager.createReaderFor(file));
    if (formatReader) {
        juce::BigInteger midiNotes;
        midiNotes.setRange(0, 128, true);  // Enable all MIDI notes
        auto* sound = new juce::SamplerSound("SampleSound", *formatReader, midiNotes, 60, 0, 0.1, 10);  // Middle C as root note
        synth.addSound(sound);
    } else {
        DBG("Failed to load sample from path: " << path);
    }
}

void Sampler::setVolume(float newVolume) {
    volume = std::clamp(newVolume, 0.0f, 1.0f);
    DBG("Volume set to: " << volume);
}

void Sampler::handleNoteOn(int midiNoteNumber, float velocity) {
    synth.noteOn(1, midiNoteNumber, velocity);
}

void Sampler::handleNoteOff(int midiNoteNumber, float velocity) {
    synth.noteOff(1, midiNoteNumber, velocity, true);
}

void Sampler::renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples) {
    midiCollector.removeNextBlockOfMessages(midiMessages, numSamples);
    synth.renderNextBlock(buffer, midiMessages, startSample, numSamples);
    buffer.applyGainRamp(startSample, numSamples, volume, volume);
}
