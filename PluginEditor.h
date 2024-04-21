#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include <JuceHeader.h>
class NewProjectAudioProcessor;  // Forward declaration

class NewProjectAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor(NewProjectAudioProcessor&);
    virtual ~NewProjectAudioProcessorEditor();
    void setWaveform(int type);
    void setSynthVolume(float volume);
    void setSampleVolume(float volume);
    std::vector<juce::File> getSampleFiles() const;
    void paint(juce::Graphics&) override;
    void resized() override;
    void updateSampleList(); // Function to populate sample choices
    void updateSynthWaveformList(); // Function to populate waveform choices

private:
    NewProjectAudioProcessor& audioProcessor;
    juce::ComboBox sampleSelector;  // ComboBox for selecting samples
    juce::ComboBox waveformSelector; // ComboBox for selecting synth waveform
    juce::Slider synthVolumeSlider;  // Slider for adjusting synthesizer volume
    juce::Slider sampleVolumeSlider; // Slider for adjusting sample volume
    juce::Slider unisonSizeSlider;   // Slider for unison size
    juce::Slider unisonDetuneSlider; // Slider for unison detune
    juce::Label unisonSizeLabel;     // Label for unison size slider
    juce::Label unisonDetuneLabel;   // Label for unison detune slider

    void setupUnisonControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};

#endif // PLUGINEDITOR_H_INCLUDED
