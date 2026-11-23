/*
    VariMuCompressor.cpp — Tube variable-gain compressor model
*/

#include "VariMuCompressor.h"

namespace vybecode {

void VariMuCompressor::prepare(const ModuleSpec& spec) {
    sampleRate = spec.sampleRate;
    detectorEnv = 0.0f;
    gainReductionDb = 0.0f;
    shelfState[0] = shelfState[1] = 0.0f;

    driveSmoother.reset(sampleRate, 0.02);
    driveSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(4.0f));
    outputSmoother.reset(sampleRate, 0.02);
    outputSmoother.setCurrentAndTargetValue(1.0f);
}

void VariMuCompressor::process(juce::AudioBuffer<float>& buffer) {
    const float attackMs  = attackParam  ? attackParam->load()  : 40.0f;
    const float releaseMs = releaseParam ? releaseParam->load() : 400.0f;
    const float warmth    = warmthParam  ? warmthParam->load()  : 0.3f;

    if (driveParam != nullptr)
        driveSmoother.setTargetValue(juce::Decibels::decibelsToGain(driveParam->load()));
    if (outputParam != nullptr)
        outputSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputParam->load()));

    const float attackCoeff  = std::exp(-1.0f / (float(sampleRate) * attackMs  * 0.001f));
    const float releaseCoeff = std::exp(-1.0f / (float(sampleRate) * releaseMs * 0.001f));
    const float envCoeff     = std::exp(-1.0f / (float(sampleRate) * 0.008f));

    // Transformer low-shelf bloom at ~120 Hz, up to +2.5 dB with warmth
    const float shelfCoeff = std::exp(-2.0f * juce::MathConstants<float>::pi * 120.0f
                                      / float(sampleRate));
    const float shelfGain = (juce::Decibels::decibelsToGain(2.5f * warmth) - 1.0f);

    const int numChannels = juce::jmin(buffer.getNumChannels(), 2);
    const int numSamples  = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample) {
        const float drive = driveSmoother.getNextValue();
        const float out   = outputSmoother.getNextValue();

        // ── Tube transconductance gain remap (no threshold/ratio) ──
        // The detector level (feedback — POST-gain, from the previous samples)
        // maps through a soft continuous knee: the effective ratio rises
        // smoothly from 1.5:1 toward ~8:1 as the tube is driven harder.
        const float detDb = (detectorEnv > 1e-10f)
            ? 20.0f * std::log10(detectorEnv) : -120.0f;
        constexpr float kneeStartDb = -30.0f; // where the mu starts to move
        float targetGrDb = 0.0f;
        const float over = detDb - kneeStartDb;
        if (over > 0.0f) {
            // Smoothly increasing slope: ratio(over) = 1.5 -> ~8 with level
            const float ratio = 1.5f + 6.5f * (1.0f - std::exp(-over / 18.0f));
            targetGrDb = -(over - over / ratio);
        }

        if (targetGrDb < gainReductionDb)
            gainReductionDb = attackCoeff * gainReductionDb
                            + (1.0f - attackCoeff) * targetGrDb;
        else
            gainReductionDb = releaseCoeff * gainReductionDb
                            + (1.0f - releaseCoeff) * targetGrDb;

        const float gr = juce::Decibels::decibelsToGain(gainReductionDb);

        float postPeak = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = buffer.getWritePointer(ch);
            float x = data[sample] * drive * gr;

            // Warmth: level-dependent 2nd harmonic (even-order tube/iron)
            x += warmth * 0.12f * x * std::abs(x);

            // Transformer low-shelf bloom
            shelfState[ch] = shelfCoeff * shelfState[ch]
                           + (1.0f - shelfCoeff) * x;
            x += shelfGain * shelfState[ch];

            postPeak = std::max(postPeak, std::abs(x));
            data[sample] = x * out;
        }

        // Feedback topology: the detector reads the post-gain signal
        detectorEnv = envCoeff * detectorEnv + (1.0f - envCoeff) * postPeak;
    }
}

void VariMuCompressor::reset() {
    detectorEnv = 0.0f;
    gainReductionDb = 0.0f;
    shelfState[0] = shelfState[1] = 0.0f;
    driveSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(4.0f));
    outputSmoother.setCurrentAndTargetValue(1.0f);
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
VariMuCompressor::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "input_drive", 1},
        prefix + "Input",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f),
        4.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "attack_ms", 1},
        prefix + "Attack",
        juce::NormalisableRange<float>(10.0f, 100.0f, 0.1f, 0.5f),
        40.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "release_ms", 1},
        prefix + "Release",
        juce::NormalisableRange<float>(100.0f, 2000.0f, 1.0f, 0.5f),
        400.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "warmth", 1},
        prefix + "Warmth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.3f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "output", 1},
        prefix + "Output",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    return params;
}

void VariMuCompressor::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                         const juce::String& prefix) {
    driveParam   = getParam(apvts, prefix, "input_drive");
    attackParam  = getParam(apvts, prefix, "attack_ms");
    releaseParam = getParam(apvts, prefix, "release_ms");
    warmthParam  = getParam(apvts, prefix, "warmth");
    outputParam  = getParam(apvts, prefix, "output");
}

} // namespace vybecode
