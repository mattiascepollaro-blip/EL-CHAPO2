#include "PluginEditor.h"

ELChapo2Editor::ELChapo2Editor(ELChapo2Processor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    input_gain_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    input_gain_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    input_gain_dbSlider.setName("input_gain_db");
    addAndMakeVisible(input_gain_dbSlider);
    sat1_gain1Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat1_gain1Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat1_gain1Slider.setName("sat1_gain1");
    addAndMakeVisible(sat1_gain1Slider);
    sat1_gain2Slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat1_gain2Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat1_gain2Slider.setName("sat1_gain2");
    addAndMakeVisible(sat1_gain2Slider);
    sat1_biasSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat1_biasSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat1_biasSlider.setName("sat1_bias");
    addChildComponent(sat1_biasSlider);
    sat1_sagSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat1_sagSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat1_sagSlider.setName("sat1_sag");
    addChildComponent(sat1_sagSlider);
    sat1_outputSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat1_outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat1_outputSlider.setName("sat1_output");
    addAndMakeVisible(sat1_outputSlider);
    hpf_frequencySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hpf_frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    hpf_frequencySlider.setName("hpf_frequency");
    addAndMakeVisible(hpf_frequencySlider);
    hpf_gain_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hpf_gain_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    hpf_gain_dbSlider.setName("hpf_gain_db");
    addAndMakeVisible(hpf_gain_dbSlider);
    hpf_qSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hpf_qSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    hpf_qSlider.setName("hpf_q");
    addChildComponent(hpf_qSlider);
    envfol_attack_msSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    envfol_attack_msSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    envfol_attack_msSlider.setName("envfol_attack_ms");
    addAndMakeVisible(envfol_attack_msSlider);
    envfol_release_msSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    envfol_release_msSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    envfol_release_msSlider.setName("envfol_release_ms");
    addAndMakeVisible(envfol_release_msSlider);
    envfol_sensitivitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    envfol_sensitivitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    envfol_sensitivitySlider.setName("envfol_sensitivity");
    addChildComponent(envfol_sensitivitySlider);
    comp_input_driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    comp_input_driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    comp_input_driveSlider.setName("comp_input_drive");
    addAndMakeVisible(comp_input_driveSlider);
    comp_attack_msSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    comp_attack_msSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    comp_attack_msSlider.setName("comp_attack_ms");
    addChildComponent(comp_attack_msSlider);
    comp_release_msSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    comp_release_msSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    comp_release_msSlider.setName("comp_release_ms");
    addChildComponent(comp_release_msSlider);
    comp_warmthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    comp_warmthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    comp_warmthSlider.setName("comp_warmth");
    addChildComponent(comp_warmthSlider);
    comp_outputSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    comp_outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    comp_outputSlider.setName("comp_output");
    addAndMakeVisible(comp_outputSlider);
    sat2_driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat2_driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat2_driveSlider.setName("sat2_drive");
    addAndMakeVisible(sat2_driveSlider);
    sat2_magnetizationSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat2_magnetizationSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat2_magnetizationSlider.setName("sat2_magnetization");
    addChildComponent(sat2_magnetizationSlider);
    sat2_low_freq_boostSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat2_low_freq_boostSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat2_low_freq_boostSlider.setName("sat2_low_freq_boost");
    addChildComponent(sat2_low_freq_boostSlider);
    sat2_mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sat2_mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    sat2_mixSlider.setName("sat2_mix");
    addAndMakeVisible(sat2_mixSlider);
    makeup_gain_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeup_gain_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    makeup_gain_dbSlider.setName("makeup_gain_db");
    addAndMakeVisible(makeup_gain_dbSlider);
    output_gain_dbSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    output_gain_dbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    output_gain_dbSlider.setName("output_gain_db");
    addAndMakeVisible(output_gain_dbSlider);
    addAndMakeVisible(advancedToggle);
    advancedToggle.onClick = [this] { showAdvanced = ! showAdvanced; advancedToggle.setButtonText(showAdvanced ? "Advanced  -" : "Advanced  +"); setSize(getWidth(), showAdvanced ? 752 : 472); };


    input_gain_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "input_gain_db", input_gain_dbSlider);
    sat1_gain1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat1_gain1", sat1_gain1Slider);
    sat1_gain2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat1_gain2", sat1_gain2Slider);
    sat1_biasAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat1_bias", sat1_biasSlider);
    sat1_sagAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat1_sag", sat1_sagSlider);
    sat1_outputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat1_output", sat1_outputSlider);
    hpf_frequencyAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "hpf_frequency", hpf_frequencySlider);
    hpf_gain_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "hpf_gain_db", hpf_gain_dbSlider);
    hpf_qAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "hpf_q", hpf_qSlider);
    envfol_attack_msAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "envfol_attack_ms", envfol_attack_msSlider);
    envfol_release_msAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "envfol_release_ms", envfol_release_msSlider);
    envfol_sensitivityAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "envfol_sensitivity", envfol_sensitivitySlider);
    comp_input_driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "comp_input_drive", comp_input_driveSlider);
    comp_attack_msAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "comp_attack_ms", comp_attack_msSlider);
    comp_release_msAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "comp_release_ms", comp_release_msSlider);
    comp_warmthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "comp_warmth", comp_warmthSlider);
    comp_outputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "comp_output", comp_outputSlider);
    sat2_driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat2_drive", sat2_driveSlider);
    sat2_magnetizationAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat2_magnetization", sat2_magnetizationSlider);
    sat2_low_freq_boostAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat2_low_freq_boost", sat2_low_freq_boostSlider);
    sat2_mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "sat2_mix", sat2_mixSlider);
    makeup_gain_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "makeup_gain_db", makeup_gain_dbSlider);
    output_gain_dbAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "output_gain_db", output_gain_dbSlider);


    setSize(520, 472);
}

ELChapo2Editor::~ELChapo2Editor() {}

void ELChapo2Editor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));


    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(20.0f));
    g.drawFittedText("ELChapo2", getLocalBounds().removeFromTop(40),
                     juce::Justification::centred, 1);
}

void ELChapo2Editor::resized()
{
    auto area = getLocalBounds().reduced(16);
    area.removeFromTop(44); // Title space

    const int cols = 4;
    const int rowH = 90;
    const int colW = area.getWidth() / cols;
    int col = 0, row = 0;

    input_gain_dbSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    sat1_gain1Slider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    sat1_gain2Slider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    sat1_outputSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    hpf_frequencySlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    hpf_gain_dbSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    envfol_attack_msSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    envfol_release_msSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    comp_input_driveSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    comp_outputSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    sat2_driveSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    sat2_mixSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    makeup_gain_dbSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }
    output_gain_dbSlider.setBounds(col * colW, area.getY() + row * rowH, colW, rowH);
    if (++col >= cols) { col = 0; ++row; }

    const int advTop = area.getY() + 4 * rowH;
    advancedToggle.setBounds(area.getX(), advTop, 150, 26);
    int sCol = 0, sRow = 0;
    const int sTop = advTop + 32;
    sat1_biasSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    sat1_biasSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    sat1_sagSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    sat1_sagSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    hpf_qSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    hpf_qSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    envfol_sensitivitySlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    envfol_sensitivitySlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    comp_attack_msSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    comp_attack_msSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    comp_release_msSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    comp_release_msSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    comp_warmthSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    comp_warmthSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    sat2_magnetizationSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    sat2_magnetizationSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }
    sat2_low_freq_boostSlider.setBounds(sCol * colW, sTop + sRow * rowH, colW, rowH);
    sat2_low_freq_boostSlider.setVisible(showAdvanced);
    if (++sCol >= cols) { sCol = 0; ++sRow; }

}
