/*
    TubePreamp.cpp — Two cascaded triode gain stages
*/

#include "TubePreamp.h"

namespace vybecode {

void TubePreamp::prepare(const ModuleSpec& spec) {
    currentSampleRate = spec.sampleRate;

    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
        static_cast<size_t>(juce::jmax(1, spec.numChannels)), 1,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false);
    oversampler->initProcessing(static_cast<size_t>(spec.maxBlockSize));

    gain1Smoother.reset(spec.sampleRate, 0.02);
    gain1Smoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(12.0f));
    gain2Smoother.reset(spec.sampleRate, 0.02);
    gain2Smoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(6.0f));
    outputSmoother.reset(spec.sampleRate, 0.02);
    outputSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(-6.0f));

    for (int ch = 0; ch < 2; ++ch) {
        stage1[ch] = StageState{};
        stage2[ch] = StageState{};
    }
    sagEnv = 0.0f;
}

void TubePreamp::process(juce::AudioBuffer<float>& buffer) {
    const float bias = biasParam ? biasParam->load() : 0.0f;
    const float sag  = sagParam  ? sagParam->load()  : 0.2f;

    if (gain1Param != nullptr)
        gain1Smoother.setTargetValue(juce::Decibels::decibelsToGain(gain1Param->load()));
    if (gain2Param != nullptr)
        gain2Smoother.setTargetValue(juce::Decibels::decibelsToGain(gain2Param->load()));
    if (outputParam != nullptr)
        outputSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputParam->load()));

    // Oversampled rate for the nonlinear loop
    const float osRate = static_cast<float>(currentSampleRate) * 2.0f;

    // 10 Hz interstage coupling highpass
    const float hpCoeff = std::exp(-2.0f * juce::MathConstants<float>::pi * 10.0f / osRate);
    // Dynamic bias charge: fast charge (~5 ms), slow discharge (~80 ms) —
    // hard drive pushes the operating point down, then it recovers (blocking).
    const float chargeUp   = std::exp(-1.0f / (osRate * 0.005f));
    const float chargeDown = std::exp(-1.0f / (osRate * 0.080f));
    // Supply sag envelope ~50 ms both ways
    const float sagCoeff = std::exp(-1.0f / (osRate * 0.050f));

    juce::dsp::AudioBlock<float> block(buffer);
    auto upBlock = oversampler->processSamplesUp(block);

    const int upChannels = juce::jmin(static_cast<int>(upBlock.getNumChannels()), 2);
    const auto upSamples = upBlock.getNumSamples();

    for (size_t s = 0; s < upSamples; ++s) {
        float g1 = gain1Smoother.getNextValue();
        float g2 = gain2Smoother.getNextValue();

        // Supply droop: a slow envelope of total output level reduces gain
        float sagGain = 1.0f / (1.0f + sag * sagEnv * 1.5f);
        float levelAccum = 0.0f;

        for (int ch = 0; ch < upChannels; ++ch) {
            float* data = upBlock.getChannelPointer(static_cast<size_t>(ch));
            float x = data[s];

            // ── Stage 1 ───────────────────────────────────────
            auto& s1 = stage1[ch];
            float drive1 = x * g1 * sagGain;
            // Dynamic bias: signal level charges the cathode network
            float lvl1 = std::abs(drive1);
            s1.biasCharge = (lvl1 > s1.biasCharge)
                ? chargeUp   * s1.biasCharge + (1.0f - chargeUp)   * lvl1
                : chargeDown * s1.biasCharge + (1.0f - chargeDown) * lvl1;
            float y1 = triode(drive1 - s1.biasCharge * 0.10f, bias);
            // Coupling highpass (10 Hz) into stage 2
            float hp1 = y1 - s1.hpPrevIn + hpCoeff * s1.hpState;
            s1.hpState = hp1;
            s1.hpPrevIn = y1;

            // ── Stage 2 ───────────────────────────────────────
            auto& s2 = stage2[ch];
            float drive2 = hp1 * g2 * sagGain;
            float lvl2 = std::abs(drive2);
            s2.biasCharge = (lvl2 > s2.biasCharge)
                ? chargeUp   * s2.biasCharge + (1.0f - chargeUp)   * lvl2
                : chargeDown * s2.biasCharge + (1.0f - chargeDown) * lvl2;
            float y2 = triode(drive2 - s2.biasCharge * 0.10f, -bias * 0.5f);
            float hp2 = y2 - s2.hpPrevIn + hpCoeff * s2.hpState;
            s2.hpState = hp2;
            s2.hpPrevIn = y2;

            data[s] = hp2;
            levelAccum += std::abs(hp2);
        }

        // Update the shared supply-sag envelope from the summed output level
        float target = levelAccum / static_cast<float>(juce::jmax(1, upChannels));
        sagEnv = sagCoeff * sagEnv + (1.0f - sagCoeff) * target;
    }

    oversampler->processSamplesDown(block);

    // Output trim + final soft ceiling (analog-style brick-wall, RT-safe).
    // std::tanh asymptotes to +/-1.0, so the module's output peak can never
    // exceed 1.0 regardless of gain1/gain2/output. Effectively transparent
    // below ~0.7 (a glue strip should never blow up its bus).
    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();
    for (int sample = 0; sample < numSamples; ++sample) {
        float out = outputSmoother.getNextValue();
        for (int ch = 0; ch < numChannels; ++ch) {
            float y = buffer.getWritePointer(ch)[sample] * out;
            buffer.getWritePointer(ch)[sample] = std::tanh(y);
        }
    }
}

void TubePreamp::reset() {
    if (oversampler) oversampler->reset();
    for (int ch = 0; ch < 2; ++ch) {
        stage1[ch] = StageState{};
        stage2[ch] = StageState{};
    }
    sagEnv = 0.0f;
    gain1Smoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(12.0f));
    gain2Smoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(6.0f));
    outputSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(-6.0f));
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
TubePreamp::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "gain1", 1},
        prefix + "Stage 1 Gain",
        juce::NormalisableRange<float>(0.0f, 40.0f, 0.1f),
        12.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "gain2", 1},
        prefix + "Stage 2 Gain",
        juce::NormalisableRange<float>(0.0f, 40.0f, 0.1f),
        6.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "bias", 1},
        prefix + "Bias",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "sag", 1},
        prefix + "Sag",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.2f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "output", 1},
        prefix + "Output",
        juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
        -6.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    return params;
}

void TubePreamp::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                   const juce::String& prefix) {
    gain1Param  = getParam(apvts, prefix, "gain1");
    gain2Param  = getParam(apvts, prefix, "gain2");
    biasParam   = getParam(apvts, prefix, "bias");
    sagParam    = getParam(apvts, prefix, "sag");
    outputParam = getParam(apvts, prefix, "output");
}

} // namespace vybecode
