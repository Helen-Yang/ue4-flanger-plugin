#include "MyAudioSourceEffects.h"

void FAudioVolumeScale::Init(const FSoundEffectSourceInitData& InitData)
{
	bIsActive = true;
	NumChannels = InitData.NumSourceChannels;
}

void FAudioVolumeScale::OnPresetChanged()
{
	UAudioVolumeScalePreset* VolumeScalePreset = CastChecked<UAudioVolumeScalePreset>(Preset);
	FAudioVolumeScaleSettings Settings = VolumeScalePreset->GetSettings();;

	VolumeScale.SetVolume(Settings.Volume);
}

void FAudioVolumeScale::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	// Check out the InData struct that's passed in to see all the stuff source effects can respond to.
	// For this demo, we're just feeding the audio to our static library for processing
	VolumeScale.ProcessAudio(InData.InputSourceEffectBufferPtr, OutAudioBufferData, InData.NumSamples);
}

void UAudioVolumeScalePreset::SetSettings(const FAudioVolumeScaleSettings& InSettings)
{
	UpdateSettings(InSettings);
}

// Flanger code here!
void FFlanger::Init(const FSoundEffectSourceInitData& InitData)
{
	bIsActive = true;
	Delays.AddDefaulted(InitData.NumSourceChannels);

	for (int32 i = 0; i < InitData.NumSourceChannels; ++i)
	{
		// note: buffer length in seconds was chosen because flanger delay effects 
		// are on the order of low tens of milliseconds (usually < 20 ms)
		Delays[i].Init(InitData.SampleRate, 0.05f);
	}
}

void FFlanger::OnPresetChanged()
{
	UFlangerPreset* FlangerPreset = CastChecked<UFlangerPreset>(Preset);
	SettingsCopy = FlangerPreset->GetSettings();

	Osc.SetFrequency(SettingsCopy.LFOFreq);
	const float sample = Osc.NextSample();

	for (Audio::FDelay& Delay : Delays)
	{
		Delay.SetEasedDelayMsec(SettingsCopy.LFOAmplitude * sample + SettingsCopy.CenterDelay, true);
	}
}

void FFlanger::ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData)
{
	int32 NumChannels = Delays.Num();
	for (int32 SampleIndex = 0; SampleIndex < InData.NumSamples; SampleIndex += NumChannels)
	{
		const float sample = Osc.NextSample();

		for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
		{
			Audio::FDelay& Delay = Delays[ChannelIndex];
			Delay.SetEasedDelayMsec(SettingsCopy.LFOAmplitude * sample + SettingsCopy.CenterDelay, true);

			const float DrySample = SettingsCopy.DryAmount * InData.InputSourceEffectBufferPtr[SampleIndex + ChannelIndex];
			const float WetSample = SettingsCopy.WetAmount * Delay.ProcessAudioSample(InData.InputSourceEffectBufferPtr[SampleIndex + ChannelIndex]);

			OutAudioBufferData[SampleIndex + ChannelIndex] = DrySample + WetSample;
		}
	}
}

void UFlangerPreset::SetSettings(const FFlangerSettings& InSettings)
{
	UpdateSettings(InSettings);
}