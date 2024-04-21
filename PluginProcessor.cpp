#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Sampler.h"
#include "SynthVoice.h"
#include <juce_core/juce_core.h>


NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
      mixParameter(new juce::AudioParameterFloat("mix", "Mix", 0.0f, 1.0f, 0.5f))
#endif
{
    addParameter(mixParameter.get());
    scanSamplesDirectory("/Volumes/Macintosh HD/Users/denisfalencik/desktop/XCODE/TRIAL/NewProject/SAMPLES");
}

NewProjectAudioProcessor::~NewProjectAudioProcessor() {}

const juce::String NewProjectAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const {
    return true;
}

bool NewProjectAudioProcessor::producesMidi() const {
    return false;
}

bool NewProjectAudioProcessor::isMidiEffect() const {
    return false;
}

double NewProjectAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms() {
    return 1;
}

int NewProjectAudioProcessor::getCurrentProgram() {
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram(int index) {}

const juce::String NewProjectAudioProcessor::getProgramName(int index) {
    return {};
}

void NewProjectAudioProcessor::changeProgramName(int index, const juce::String& newName) {}

bool NewProjectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    // Example: Only supporting stereo input and output
    if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() &&
        (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled() ||
         layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo())) {
        return true;
    }
    return false;
}


void NewProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    wavetableSynth.prepareToPlay(sampleRate, samplesPerBlock);
    sampler.prepareToPlay(sampleRate, samplesPerBlock);
}

void NewProjectAudioProcessor::releaseResources() {
    wavetableSynth.releaseResources();
    sampler.releaseResources();
}

void NewProjectAudioProcessor::setWaveform(int type) {
    if (type >= 0 && type < static_cast<int>(WavetableSynthesizer::Waveform::NumWaveforms)) {
        wavetableSynth.setWaveform(static_cast<WavetableSynthesizer::Waveform>(type));
    } else {
        DBG("Invalid waveform type specified");
    }
}

void NewProjectAudioProcessor::setSynthVolume(float volume) {
    wavetableSynth.setVolume(volume);
}

void NewProjectAudioProcessor::setSampleVolume(float volume) {
    sampler.setVolume(volume);
}

void NewProjectAudioProcessor::setUnisonSize(int size) {
    wavetableSynth.setUnisonSize(size);  // Assuming wavetableSynth manages the voices
}

void NewProjectAudioProcessor::setDetuneAmount(float amount) {
    wavetableSynth.setDetuneAmount(amount);  // Assuming wavetableSynth manages the voices
}


std::vector<juce::File> NewProjectAudioProcessor::getSampleFiles() const {
    return sampleFiles; // Ensure `sampleFiles` is a member of this class
}

void NewProjectAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    buffer.clear();  // Clear buffer at the start of processing

    juce::AudioBuffer<float> synthBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> sampleBuffer(buffer.getNumChannels(), buffer.getNumSamples());

    synthBuffer.clear();
    sampleBuffer.clear();

    // Handle MIDI messages
    for (const auto midiMessage : midiMessages) {
        auto message = midiMessage.getMessage();
        if (message.isNoteOn() || message.isNoteOff()) {
            float velocity = message.getFloatVelocity();
            int noteNumber = message.getNoteNumber();
            wavetableSynth.handleNoteOn(noteNumber, velocity);  // Process note-on
            sampler.handleNoteOn(noteNumber, velocity);  // Process note-on
            wavetableSynth.handleNoteOff(noteNumber, velocity);  // Process note-off
            sampler.handleNoteOff(noteNumber, velocity);  // Process note-off
        }
    }

    // Render audio blocks for both synthesizer and sampler
    wavetableSynth.renderNextBlock(synthBuffer, midiMessages, 0, buffer.getNumSamples());
    sampler.renderNextBlock(sampleBuffer, midiMessages, 0, buffer.getNumSamples());

    // Mix synthesizer and sampler outputs
    float mixLevel = *mixParameter;
    buffer.addFrom(0, 0, synthBuffer, 0, 0, buffer.getNumSamples(), mixLevel);
    buffer.addFrom(0, 0, sampleBuffer, 0, 0, buffer.getNumSamples(), 1.0f - mixLevel);
}

void NewProjectAudioProcessor::scanSamplesDirectory(const juce::String& path) {
    juce::File directory(path);
    if (directory.exists() && directory.isDirectory()) {
        // Initialize the ranged directory iterator to iterate through WAV files
        juce::RangedDirectoryIterator iter(directory, true, "*.wav");

        // Use a range-based for loop to process each file
        for (auto& entry : iter) {
            const auto& file = entry.getFile();  // Access the file from the DirectoryEntry object
            if (file.existsAsFile()) { // Ensure the item is a file
                sampleFiles.push_back(file); // Add the file to the list
            }
        }
    }
}










void NewProjectAudioProcessor::loadSample(const juce::String& path) {
    juce::File file(path);
    if (file.existsAsFile()) {
        currentSampleFile = file;
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

        if (reader) {
            sampleBuffer.setSize(reader->numChannels, static_cast<int>(reader->lengthInSamples));
            reader->read(&sampleBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
            samplePosition = 0;
        } else {
            DBG("Failed to read sample");
        }
    } else {
        DBG("File does not exist: " + path);
    }
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor() {
    return new NewProjectAudioProcessorEditor(*this);
}

bool NewProjectAudioProcessor::hasEditor() const {
    return true;
}

void NewProjectAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // Optionally implement state saving
}

void NewProjectAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // Optionally implement state loading
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new NewProjectAudioProcessor();
}
