#include "PluginProcessor.h"  // Ensure this is included before PluginEditor.h
#include "PluginEditor.h"

NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor(NewProjectAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p) {
    setSize(400, 300);
    // Initialize additional UI components here if needed
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor() {
    // Clean up any resources
}

void NewProjectAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
    g.setFont(15.0f);
    g.setColour(juce::Colours::white);
    g.drawText("Audio Processor Editor", getLocalBounds(), juce::Justification::centred, true);
}

void NewProjectAudioProcessorEditor::setupUnisonControls() {
    addAndMakeVisible(unisonSizeSlider);
    unisonSizeSlider.setRange(1, 8, 1);
    unisonSizeSlider.setTextValueSuffix(" Voices");
    unisonSizeSlider.onValueChange = [this] {
        audioProcessor.setUnisonSize(static_cast<int>(unisonSizeSlider.getValue()));
        };
    
    addAndMakeVisible(unisonDetuneSlider);
        unisonDetuneSlider.setRange(0.0, 0.5, 0.01); // Range in semitones
        unisonDetuneSlider.setTextValueSuffix(" Semitones");
        unisonDetuneSlider.onValueChange = [this] {
            audioProcessor.setDetuneAmount(unisonDetuneSlider.getValue());
        };

        addAndMakeVisible(unisonSizeLabel);
        unisonSizeLabel.setText("Unison Size", juce::dontSendNotification);
        unisonSizeLabel.attachToComponent(&unisonSizeSlider, true);

        addAndMakeVisible(unisonDetuneLabel);
        unisonDetuneLabel.setText("Unison Detune", juce::dontSendNotification);
        unisonDetuneLabel.attachToComponent(&unisonDetuneSlider, true);
    }

void NewProjectAudioProcessorEditor::resized() {
    unisonSizeSlider.setBounds(10, 30, getWidth() - 20, 20);
    unisonDetuneSlider.setBounds(10, 60, getWidth() - 20, 20);
}

void NewProjectAudioProcessorEditor::setWaveform(int type) {
    audioProcessor.setWaveform(type);
}

void NewProjectAudioProcessorEditor::setSynthVolume(float volume) {
    audioProcessor.setSynthVolume(volume);
}

void NewProjectAudioProcessorEditor::setSampleVolume(float volume) {
    audioProcessor.setSampleVolume(volume);
}

std::vector<juce::File> NewProjectAudioProcessorEditor::getSampleFiles() const {
    return audioProcessor.getSampleFiles();
}

void NewProjectAudioProcessorEditor::updateSampleList() {
    sampleSelector.clear();
    auto files = getSampleFiles();
    for (auto& file : files) {
        sampleSelector.addItem(file.getFileName(), sampleSelector.getNumItems() + 1);
    }
}

void NewProjectAudioProcessorEditor::updateSynthWaveformList() {
    waveformSelector.clear();
    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Square", 2);
    waveformSelector.addItem("Triangle", 3);
    waveformSelector.addItem("Sawtooth", 4);
    waveformSelector.setSelectedItemIndex(0);  // Default to first waveform
}
