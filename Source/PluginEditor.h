#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class ELChapo2Editor : public juce::AudioProcessorEditor
{
public:
    explicit ELChapo2Editor(ELChapo2Processor&);
    ~ELChapo2Editor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ELChapo2Processor& processorRef;

    // ── Controls ─────────────────────────────────────────
    juce::Slider input_gain_dbSlider;
    juce::Slider sat1_gain1Slider;
    juce::Slider sat1_gain2Slider;
    juce::Slider sat1_biasSlider;
    juce::Slider sat1_sagSlider;
    juce::Slider sat1_outputSlider;
    juce::Slider hpf_frequencySlider;
    juce::Slider hpf_gain_dbSlider;
    juce::Slider hpf_qSlider;
    juce::Slider envfol_attack_msSlider;
    juce::Slider envfol_release_msSlider;
    juce::Slider envfol_sensitivitySlider;
    juce::Slider comp_input_driveSlider;
    juce::Slider comp_attack_msSlider;
    juce::Slider comp_release_msSlider;
    juce::Slider comp_warmthSlider;
    juce::Slider comp_outputSlider;
    juce::Slider sat2_driveSlider;
    juce::Slider sat2_magnetizationSlider;
    juce::Slider sat2_low_freq_boostSlider;
    juce::Slider sat2_mixSlider;
    juce::Slider makeup_gain_dbSlider;
    juce::Slider output_gain_dbSlider;
    juce::TextButton advancedToggle { "Advanced  +" };
    bool showAdvanced { false };


    // ── Attachments (declared AFTER sliders for correct destruction order) ──
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> input_gain_dbAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat1_gain1Attach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat1_gain2Attach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat1_biasAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat1_sagAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat1_outputAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpf_frequencyAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpf_gain_dbAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpf_qAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envfol_attack_msAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envfol_release_msAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envfol_sensitivityAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp_input_driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp_attack_msAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp_release_msAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp_warmthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp_outputAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat2_driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat2_magnetizationAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat2_low_freq_boostAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sat2_mixAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeup_gain_dbAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> output_gain_dbAttach;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ELChapo2Editor)
};
