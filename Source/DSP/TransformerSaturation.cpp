/*
    TransformerSaturation.cpp — Audio transformer with hysteresis
*/

#include "TransformerSaturation.h"

namespace vybecode {

void TransformerSaturation::prepare(const ModuleSpec& spec) {
    currentSampleRate = spec.sampleRate;

    driveSmoother.reset(spec.sampleRate, 0.02);
    magnetizationSmoother.reset(spec.sampleRate, 0.02);
    lowFreqBoostSmoother.reset(spec.sampleRate, 0.02);
    mixSmoother.reset(spec.sampleRate, 0.02);

    driveSmoother.setCurrentAndTargetValue(0.5f);
    magnetizationSmoother.setCurrentAndTargetValue(0.5f);
    lowFreqBoostSmoother.setCurrentAndTargetValue(0.0f);
    mixSmoother.setCurrentAndTargetValue(1.0f);

    for (int ch = 0; ch < 2; ++ch) {
        chState[ch].M = 0.0;
        chState[ch].H_prev = 0.0;
        chState[ch].dMdt_prev = 0.0;
        chState[ch].lsX1 = 0.0; chState[ch].lsX2 = 0.0;
        chState[ch].lsY1 = 0.0; chState[ch].lsY2 = 0.0;
    }

    updateLowShelf(0.0f, 120.0f);
}

void TransformerSaturation::updateLowShelf(float boostDb, float freq) {
    // Second-order low shelf filter design
    double A = std::pow(10.0, boostDb / 40.0);
    double w0 = 2.0 * juce::MathConstants<double>::pi * freq / currentSampleRate;
    double sinw = std::sin(w0);
    double cosw = std::cos(w0);
    double S = 1.0;  // Shelf slope
    double alphaCoeff = sinw / 2.0 * std::sqrt((A + 1.0 / A) * (1.0 / S - 1.0) + 2.0);

    double a0 = (A + 1.0) + (A - 1.0) * cosw + 2.0 * std::sqrt(A) * alphaCoeff;
    lsB0 = (A * ((A + 1.0) - (A - 1.0) * cosw + 2.0 * std::sqrt(A) * alphaCoeff)) / a0;
    lsB1 = (2.0 * A * ((A - 1.0) - (A + 1.0) * cosw)) / a0;
    lsB2 = (A * ((A + 1.0) - (A - 1.0) * cosw - 2.0 * std::sqrt(A) * alphaCoeff)) / a0;
    lsA1 = (-2.0 * ((A - 1.0) + (A + 1.0) * cosw)) / a0;
    lsA2 = ((A + 1.0) + (A - 1.0) * cosw - 2.0 * std::sqrt(A) * alphaCoeff) / a0;
}

void TransformerSaturation::process(juce::AudioBuffer<float>& buffer) {
    if (driveParam != nullptr)
        driveSmoother.setTargetValue(driveParam->load());
    if (magnetizationParam != nullptr)
        magnetizationSmoother.setTargetValue(magnetizationParam->load());
    if (lowFreqBoostParam != nullptr)
        lowFreqBoostSmoother.setTargetValue(lowFreqBoostParam->load());
    if (mixParam != nullptr)
        mixSmoother.setTargetValue(mixParam->load());

    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin(buffer.getNumChannels(), 2);

    for (int sample = 0; sample < numSamples; ++sample) {
        float drive = driveSmoother.getNextValue();
        float magnetization = magnetizationSmoother.getNextValue();
        float mix = mixSmoother.getNextValue();
        float lfBoost = lowFreqBoostSmoother.getNextValue();

        // Update low shelf periodically
        if (sample % 32 == 0)
            updateLowShelf(lfBoost * 6.0f, 120.0f);

        // Scale Jiles-Atherton parameters with magnetization control
        double localMs = Ms * (0.5 + magnetization * 1.5);
        double localK = k * (1.0 - magnetization * 0.5);

        for (int ch = 0; ch < numChannels; ++ch) {
            float* data = buffer.getWritePointer(ch);
            float dry = data[sample];

            // Drive scales input field strength
            double H = static_cast<double>(dry) * (1.0 + drive * 20.0);

            auto& st = chState[ch];

            // Simplified Jiles-Atherton differential equation
            // dM/dH = (Man - M) / (k * delta - alpha * (Man - M))
            // where Man = Ms * L((H + alpha*M) / a) is anhysteretic magnetization
            double He = H + alpha * st.M;  // Effective field
            double Man = localMs * langevin(He / a);  // Anhysteretic magnetization

            double dH = H - st.H_prev;
            double delta = (dH >= 0) ? 1.0 : -1.0;

            double denom = localK * delta - alpha * (Man - st.M);
            double dMdH = 0.0;
            if (std::abs(denom) > 1e-12)
                dMdH = (1.0 - c) * (Man - st.M) / denom + c * localMs * langevin(He / a) / a;

            // Clamp to prevent instability
            dMdH = juce::jlimit(-10.0, 10.0, dMdH);

            st.M += dMdH * dH;
            st.M = juce::jlimit(-localMs * 2.0, localMs * 2.0, st.M);
            st.H_prev = H;

            // Output is magnetization normalized to [-1, 1]
            float wet = static_cast<float>(st.M / localMs);

            // Apply low shelf filter for LF boost
            double lsOut = lsB0 * wet + lsB1 * st.lsX1 + lsB2 * st.lsX2
                           - lsA1 * st.lsY1 - lsA2 * st.lsY2;
            st.lsX2 = st.lsX1; st.lsX1 = wet;
            st.lsY2 = st.lsY1; st.lsY1 = lsOut;

            wet = static_cast<float>(lsOut);

            data[sample] = dry * (1.0f - mix) + wet * mix;
        }
    }
}

void TransformerSaturation::reset() {
    for (int ch = 0; ch < 2; ++ch) {
        chState[ch].M = 0.0;
        chState[ch].H_prev = 0.0;
        chState[ch].dMdt_prev = 0.0;
        chState[ch].lsX1 = 0.0; chState[ch].lsX2 = 0.0;
        chState[ch].lsY1 = 0.0; chState[ch].lsY2 = 0.0;
    }
    driveSmoother.setCurrentAndTargetValue(0.5f);
    magnetizationSmoother.setCurrentAndTargetValue(0.5f);
    lowFreqBoostSmoother.setCurrentAndTargetValue(0.0f);
    mixSmoother.setCurrentAndTargetValue(1.0f);
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>>
TransformerSaturation::createParameters(const juce::String& prefix) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "drive", 1},
        prefix + "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "magnetization", 1},
        prefix + "Magnetization",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "low_freq_boost", 1},
        prefix + "Low Freq Boost",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{prefix + "mix", 1},
        prefix + "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f));

    return params;
}

void TransformerSaturation::updateFromAPVTS(juce::AudioProcessorValueTreeState& apvts,
                                              const juce::String& prefix) {
    driveParam = getParam(apvts, prefix, "drive");
    magnetizationParam = getParam(apvts, prefix, "magnetization");
    lowFreqBoostParam = getParam(apvts, prefix, "low_freq_boost");
    mixParam = getParam(apvts, prefix, "mix");
}

} // namespace vybecode
