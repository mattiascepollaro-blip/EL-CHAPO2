/*
    EnvelopeFollower.cpp — Tracks amplitude of input signal
*/

#include "EnvelopeFollower.h"
#include <cmath>

namespace vybecode {

void EnvelopeFollower::prepare(const ModuleSpec& spec) {
    currentSampleRate = spec.sampleRate;
    envelopeState = 0.0f;
    currentEnvelope.store(0.0f);
}

void EnvelopeFollower::process(juce::AudioBuffer<float>& buffer) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    float attackMs  = (attackMsParam != nullptr)  ? attackMsParam->load()  : 10.0f;
    float releaseMs = (releaseMsParam != nullptr) ? releaseMsParam->load() : 100.0f;
    float sensitivity = (sensitivityParam != nullptr) ? sensitivityParam->load() : 50.0f;

    // Convert ms to one-pole coefficients
    float attackCoeff  = std::exp(-1.0f / (static_cast<float>(currentSampleRate) * attackMs * 0.001f));
    float releaseCoeff = std::exp(-1.0f / (static_cast<float>(currentSampleRate) * releaseMs * 0.001f));

    // Sensitivity maps 0-100 to gain multiplier 0.1-10.0
    float gainMult = 0.1f + (sensitivity / 100.0f) * 9.9f;

    float env = envelopeState;

    for (int sample = 0; sample < numSamples; ++sample) {
        // Sum absolute values across channels for mono envelope
        float rectified = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            rectified += std::abs(buffer.getReadPointer(ch)[sample]);
        rectified = (numChannels > 0) ? (rectified / static_cast<float>(numChannels)) : 0.0f;

        // Apply sensitivity gain
        rectified *= gainMult;

        // One-pole follower: fast attack, slow release
        if (rectified > env)
            env = attackCoeff * env + (1.0f - attackCoeff) * rectified;
        else
            env = releaseCoeff * env + (1.0f - releaseCoeff) * rectified;
    }

    envelopeState = env;
    currentEnvelope.store(env);

    // Envelope follower is a detector — it does NOT modify the audio buffer.
    // The envelope value is available via getEnvelopeValue() for modulation routing.
}

void EnvelopeFollower::reset() {
    envelopeState = 0.0f;
    currentEnvelope.store(0.0f);
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
EnvelopeFollower::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "attack_ms", 1},
        prefix + "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f),
        10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "release_ms", 1},
        prefix + "Release",
        juce::NormalisableRange<float>(1.0f, 1000.0f, 0.1f, 0.4f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "sensitivity", 1},
        prefix + "Sensitivity",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    return params;
}

void EnvelopeFollower::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                         const juce::String& prefix) {
    attackMsParam    = getParam(apvts, prefix, "attack_ms");
    releaseMsParam   = getParam(apvts, prefix, "release_ms");
    sensitivityParam = getParam(apvts, prefix, "sensitivity");
}

} // namespace vybecode
