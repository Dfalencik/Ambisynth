#pragma once

#include <JuceHeader.h>
#include "WavetableSynthesizer.h"
#include "Sampler.h"

class NewProjectAudioProcessor : public juce::AudioProcessor {
public:
    NewProjectAudioProcessor();
    virtual ~NewProjectAudioProcessor() override;

    void prepareToPlay(double sampleRate, int asamplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    void setWaveform(int type);
    void setSynthVolume(float volume);
    void setSampleVolume(float volume);
    std::vector<juce::File> getSampleFiles() const;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void scanSamplesDirectory(const juce::String& path);
    void loadSample(const juce::String& path);
    void setUnisonSize(int size);
       void setDetuneAmount(float amount);

private:
    WavetableSynthesizer wavetableSynth;
    Sampler sampler;
    std::unique_ptr<juce::AudioParameterFloat> mixParameter;
    std::vector<juce::File> sampleFiles;
    juce::File currentSampleFile;
    juce::AudioBuffer<float> sampleBuffer;
    int samplePosition = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessor)
};
