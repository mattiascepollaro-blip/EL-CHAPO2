/*
    LowShelfFilter.cpp — Low shelf EQ using juce::dsp::IIR
*/

#include "LowShelfFilter.h"

namespace vybecode {

void LowShelfFilter::prepare(const ModuleSpec& spec) {
    currentSampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec jSpec;
    jSpec.sampleRate = spec.sampleRate;
    jSpec.maximumBlockSize = static_cast<juce::uint32>(spec.maxBlockSize);
    jSpec.numChannels = static_cast<juce::uint32>(spec.numChannels);
    filter.prepare(jSpec);

    freqSmoother.reset(spec.sampleRate, 0.02);
    freqSmoother.setCurrentAndTargetValue(200.0f);
    gainSmoother.reset(spec.sampleRate, 0.02);
    gainSmoother.setCurrentAndTargetValue(0.0f);
}

void LowShelfFilter::process(juce::AudioBuffer<float>& buffer) {
    if (freqParam != nullptr)
        freqSmoother.setTargetValue(freqParam->load());
    if (gainDbParam != nullptr)
        gainSmoother.setTargetValue(gainDbParam->load());

    const float freq = freqSmoother.getCurrentValue();
    const float gainDb = gainSmoother.getCurrentValue();
    const float q = (qParam != nullptr) ? qParam->load() : 0.707f;

    // Update coefficients
    *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate, freq, q, juce::Decibels::decibelsToGain(gainDb));

    // Process block
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);

    // Advance smoothers
    freqSmoother.skip(buffer.getNumSamples());
    gainSmoother.skip(buffer.getNumSamples());
}

void LowShelfFilter::reset() {
    filter.reset();
    freqSmoother.setCurrentAndTargetValue(200.0f);
    gainSmoother.setCurrentAndTargetValue(0.0f);
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
LowShelfFilter::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "frequency", 1},
        prefix + "Frequency",
        juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f, 0.3f),
        200.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "gain_db", 1},
        prefix + "Gain",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "q", 1},
        prefix + "Q",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f),
        0.707f));

    return params;
}

void LowShelfFilter::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                      const juce::String& prefix) {
    freqParam   = getParam(apvts, prefix, "frequency");
    gainDbParam = getParam(apvts, prefix, "gain_db");
    qParam      = getParam(apvts, prefix, "q");
}

} // namespace vybecode
