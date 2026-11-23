/*
    TubePreamp.h — Two cascaded triode gain stages
    Category: Analog
    Parameters: gain1 (0-40 dB), gain2 (0-40 dB), bias (-1 to 1),
                sag (0-1), output (-24 to 6 dB)

    Clean warmth to driven preamp crunch. Each stage models grid conduction
    (a one-sided soft limit), an asymmetric Koren-style triode transfer
    (polynomial fit), and an interstage coupling highpass whose charge state
    shifts with signal level — the dynamic bias shift behind touch-responsive
    blocking distortion at high drive. Sag models supply droop as a slow
    envelope-driven gain reduction. 2x oversampled. Distinct from
    pentode_distortion (single hard stage): this is the cascaded-triode topology.
*/

#pragma once
#include "DSPModule.h"
#include <cmath>
#include <memory>

namespace vybecode {

class TubePreamp : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "tube_preamp"; }
    juce::String getModuleName() const override { return "Tube Preamp"; }

private:
    std::atomic<float>* gain1Param = nullptr;
    std::atomic<float>* gain2Param = nullptr;
    std::atomic<float>* biasParam = nullptr;
    std::atomic<float>* sagParam = nullptr;
    std::atomic<float>* outputParam = nullptr;

    juce::SmoothedValue<float> gain1Smoother;
    juce::SmoothedValue<float> gain2Smoother;
    juce::SmoothedValue<float> outputSmoother;

    double currentSampleRate = 44100.0;

    // 2x oversampling around the nonlinear stages
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    // Per-channel, per-stage state: coupling highpass + dynamic bias charge
    struct StageState {
        float hpState = 0.0f;    // coupling capacitor highpass
        float hpPrevIn = 0.0f;
        float biasCharge = 0.0f; // level-driven charge -> dynamic bias shift
    };
    StageState stage1[2];
    StageState stage2[2];

    // Supply-sag envelope (shared — one power supply)
    float sagEnv = 0.0f;

    // One triode stage: grid conduction clip + Koren-style asymmetric
    // polynomial transfer. staticBias offsets the operating point.
    static inline float triode(float x, float staticBias) {
        // Grid conduction: the grid starts drawing current when driven
        // positive — a one-sided soft limit above ~0.4.
        if (x > 0.4f)
            x = 0.4f + (x - 0.4f) / (1.0f + (x - 0.4f) * 2.5f);

        // Bias shifts the operating point hard enough to be musical: a full
        // +/-1.0 bias now swings the offset +/-0.5 (was +/-0.15), strongly
        // rebalancing the even-harmonic (v*v) asymmetry under drive.
        float v = x + staticBias * 0.5f;
        // Polynomial fit of the Koren triode curve around the operating
        // point: odd-dominant with a (slightly stronger) even term for the
        // triode's asymmetry so the bias sweep reads.
        float y = v - 0.30f * v * v - 0.18f * v * v * v;
        // A driven triode stage tops out near unity, not +/-1.5 -> no
        // per-stage runaway (the gain2 ceiling fix).
        return juce::jlimit(-1.0f, 1.0f, y);
    }
};

} // namespace vybecode
