/*
    LowShelfFilter.h — Low shelf EQ using juce::dsp::IIR
    Category: EQ
    Parameters: frequency (20-1000Hz), gain_db (-12 to +12), q (0.1-10)
*/

#pragma once
#include "DSPModule.h"

namespace vybecode {

class LowShelfFilter : public DSPModule {
public:
    void prepare(const ModuleSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>>
        createParameters(const juce::String& prefix) override;
    void updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix) override;

    juce::String getModuleID() const override { return "low_shelf"; }
    juce::String getModuleName() const override { return "Low Shelf EQ"; }

private:
    std::atomic<float>* freqParam = nullptr;
    std::atomic<float>* gainDbParam = nullptr;
    std::atomic<float>* qParam = nullptr;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> filter;
    juce::SmoothedValue<float> freqSmoother;
    juce::SmoothedValue<float> gainSmoother;
    double currentSampleRate = 44100.0;
};

} // namespace vybecode
