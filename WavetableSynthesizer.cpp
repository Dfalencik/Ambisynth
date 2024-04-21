#include "WavetableSynthesizer.h"
#include "SynthVoice.h"
#include "PluginProcessor.h"
#include "Sampler.h"



WavetableSynthesizer::WavetableSynthesizer()
: masterVolume(1.0f), voices(16) {
    fillWavetable();
    for (auto& voice : voices) {
        voice = std::make_unique<SynthVoice>();
        voice->setWavetable(wavetables);
    }
}

WavetableSynthesizer::~WavetableSynthesizer() {}

void WavetableSynthesizer::prepareToPlay(double sampleRate, int samplesPerBlock) {
    currentSampleRate = sampleRate;
    for (auto& voice : voices) {
        voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

float WavetableSynthesizer::getNextSample() {
    float output = 0.0f;
    for (auto& voice : voices) {
        if (voice->isActive()) {
            output += voice->getNextSample();
        }
    }
    return output;
}



void WavetableSynthesizer::releaseResources() {}

void WavetableSynthesizer::renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples) {
    buffer.clear();
    for (int sample = 0; sample < numSamples; ++sample) {
        float output = getNextSample() * masterVolume;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.addSample(channel, startSample + sample, output);
        }
    }
}

void WavetableSynthesizer::handleNoteOn(int noteNumber, float velocity) {
    // Try to find an inactive voice first
    for (auto& voice : voices) {
        if (!voice->isActive()) {
            voice->startNote(noteNumber, velocity);
            return; // Found an inactive voice, we're done
        }
    }

    // If all voices are active, steal the oldest voice (or a suitable candidate):
    int oldestVoiceIndex = 0; // You might want a more sophisticated way to choose a voice
    voices[oldestVoiceIndex]->stopNote(true);
    voices[oldestVoiceIndex]->startNote(noteNumber, velocity);
}


void WavetableSynthesizer::handleNoteOff(int noteNumber, float velocity) {
    for (auto& voice : voices) {
        if (voice->getNoteNumber() == noteNumber) {
            voice->stopNote(true);
            break;
        }
    }
}

void WavetableSynthesizer::setUnisonSize(int size) {
    for (auto& voice : voices) {
        voice->setUnisonSize(size);
    }
}

void WavetableSynthesizer::setDetuneAmount(float amount) {
    for (auto& voice : voices) {
        voice->setDetuneAmount(amount);
    }
}

void WavetableSynthesizer::setVolume(float volume) {
    masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

void WavetableSynthesizer::setWaveform(Waveform newWaveform) {
    currentWaveform = newWaveform;
    fillWavetable();
    for (auto& voice : voices) {
        voice->setWavetable(wavetables);
    }
}

void WavetableSynthesizer::fillWavetable() {
    wavetables[Sine].resize(tableSize);
    wavetables[Square].resize(tableSize);
    wavetables[Triangle].resize(tableSize);
    wavetables[Sawtooth].resize(tableSize);
    
    generateSineWave(wavetables[Sine]);
    generateSquareWave(wavetables[Square]);
    generateTriangleWave(wavetables[Triangle]);
    generateSawtoothWave(wavetables[Sawtooth]);
}

void WavetableSynthesizer::generateSineWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        table[i] = std::sin(2.0 * juce::MathConstants<double>::pi * i / tableSize);
    }
}

void WavetableSynthesizer::generateSquareWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        table[i] = (i < tableSize / 2) ? 1.0f : -1.0f;
    }
}

void WavetableSynthesizer::generateTriangleWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        table[i] = 2.0f * std::abs(2.0f * (i / float(tableSize)) - 1.0f) - 1.0f;
    }
}

void WavetableSynthesizer::generateSawtoothWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        table[i] = 2.0f * (i / float(tableSize)) - 1.0f;
    }
}
