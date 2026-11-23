/*
    TransformerSaturation.h — Audio transformer with hysteresis
    Category: Analog
    Parameters: drive (0-1), magnetization (0-1), low_freq_boost (0-1), mix (0-1)

    Simplified Jiles-Atherton hysteresis model for audio transformer
    saturation. Frequency-dependent: low frequencies saturate harder
    due to core flux accumulation, while high frequencies pass more
    linearly. Includes low-frequency boost from magnetizing inductance.
*/

#pragma once
#include "DSPModule.h"
#include <cmath>

namespace vybecode {

class TransformerSaturation : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "transformer_sat"; }
    juce::String getModuleName() const override { return "Transformer Saturation"; }

private:
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* magnetizationParam = nullptr;
    std::atomic<float>* lowFreqBoostParam = nullptr;
    std::atomic<float>* mixParam = nullptr;

    juce::SmoothedValue<float> driveSmoother;
    juce::SmoothedValue<float> magnetizationSmoother;
    juce::SmoothedValue<float> lowFreqBoostSmoother;
    juce::SmoothedValue<float> mixSmoother;

    double currentSampleRate = 44100.0;

    // Hysteresis state per channel
    struct HysteresisState {
        double M = 0.0;          // Magnetization
        double H_prev = 0.0;     // Previous field strength
        double dMdt_prev = 0.0;  // Previous rate of change

        // Low shelf filter state (for LF boost)
        double lsX1 = 0.0, lsX2 = 0.0;
        double lsY1 = 0.0, lsY2 = 0.0;
    };

    HysteresisState chState[2];

    // Jiles-Atherton model parameters
    double Ms = 1.0;     // Saturation magnetization
    double a = 50.0;     // Domain wall density
    double k = 30.0;     // Domain pinning
    double c = 0.8;      // Domain wall bowing
    double alpha = 1e-4;  // Inter-domain coupling

    // Langevin function L(x) = coth(x) - 1/x
    static inline double langevin(double x) {
        if (std::abs(x) < 1e-6) return x / 3.0;
        return 1.0 / std::tanh(x) - 1.0 / x;
    }

    // Low shelf filter coefficients
    double lsB0 = 1.0, lsB1 = 0.0, lsB2 = 0.0;
    double lsA1 = 0.0, lsA2 = 0.0;

    void updateLowShelf(float boostDb, float freq);
};

} // namespace vybecode
