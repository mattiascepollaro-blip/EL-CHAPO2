/*
    EnvelopeFollower.h — Tracks amplitude of input signal
    Category: Envelopes
    Parameters: attack_ms (0.1-100), release_ms (1-1000), sensitivity (0-100)
*/

#pragma once
#include "DSPModule.h"

namespace vybecode {

class EnvelopeFollower : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "env_follower"; }
    juce::String getModuleName() const override { return "Envelope Follower"; }

    // External access to current envelope value (0.0 to ~1.0)
    float getEnvelopeValue() const { return currentEnvelope.load(); }
    // Uniform accessor for modulation routing (GL-74).
    float getCurrentValue() const { return currentEnvelope.load(); }

private:
    std::atomic<float>* attackMsParam  = nullptr;
    std::atomic<float>* releaseMsParam = nullptr;
    std::atomic<float>* sensitivityParam = nullptr;

    std::atomic<float> currentEnvelope{0.0f};
    float envelopeState = 0.0f; // per-sample state (mono reduction)
    double currentSampleRate = 44100.0;
};

} // namespace vybecode
