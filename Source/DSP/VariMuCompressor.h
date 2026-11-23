/*
    VariMuCompressor.h — Tube variable-gain compressor model
    Category: Analog
    Parameters: input_drive (-12 to 24 dB), attack_ms (10-100 ms),
                release_ms (100-2000 ms), warmth (0-1), output (-12 to 12 dB)

    No threshold/ratio controls — true vari-mu behavior: the gain is a remap
    of the detector level through a tube-transconductance curve (a soft
    continuous knee whose effective ratio rises from 1.5:1 toward ~8:1 as
    drive increases), with a feedback detector topology (the envelope is
    taken POST-gain, giving the characteristic gentle self-correcting
    action). `warmth` adds level-dependent 2nd harmonic + a low-shelf bloom
    modeling the output transformer. The slowest, smoothest compressor in
    the set — distinct from fet/opto/vca.
*/

#pragma once
#include "DSPModule.h"
#include <cmath>

namespace vybecode {

class VariMuCompressor : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "vari_mu_compressor"; }
    juce::String getModuleName() const override { return "Vari-Mu Compressor"; }

private:
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    std::atomic<float>* warmthParam = nullptr;
    std::atomic<float>* outputParam = nullptr;

    double sampleRate = 44100.0;

    // Feedback detector: envelope of the POST-gain signal (max of channels)
    float detectorEnv = 0.0f;
    float gainReductionDb = 0.0f;

    // Transformer low-shelf bloom state (one-pole per channel)
    float shelfState[2] = {0.0f, 0.0f};

    juce::SmoothedValue<float> driveSmoother;
    juce::SmoothedValue<float> outputSmoother;
};

} // namespace vybecode
