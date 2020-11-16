#pragma once

#include "CoreMinimal.h" // some basic minimal includes (still a lot, but this lets us use U-stuff
#include "DSP/Delay.h"

#include "Sound/SoundEffectSource.h"  // This is the include to allow us to make a source effect
#include "AudioLibrary.h" // This is our static library!  We can do this because of our Build.cs changes to PublicIncludePaths

#include "MyAudioSourceEffects.generated.h" // Long story on this, but the native reflection works by code-generation created by the "Unreal Header Tool" which is included here


// This is a ustruct (not a U-object!) which is contained in a U-asset below (which is a U-object!).
// This uses the power of reflection to generate UI automatically with a bunch of useful properties (ranges, tooltips, etc)
USTRUCT(BlueprintType)
struct FAudioVolumeScaleSettings
{
	GENERATED_USTRUCT_BODY()

		// The volume scale to use
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float Volume = 0.2f;
};

// This is the instance of the effect. One is created for every source which is playing with the settings asset
class MYAUDIOPLUGIN_API FAudioVolumeScale : public FSoundEffectSource
{
public:
	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSourceInitData& InitData) override;

	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio render thread.
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;

protected:

	// Our third party library object
	AudioLibrary::VolumeScale VolumeScale;
	int32 NumChannels = 0;
};

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class MYAUDIOPLUGIN_API UAudioVolumeScalePreset : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	// This is a little helper macro to expose a bunch of functions easily
	EFFECT_PRESET_METHODS(AudioVolumeScale)

	// Can override the preset color in the content browser if you want
	virtual FColor GetPresetColor() const override { return FColor(127.0f, 0.0f, 0.0f); }

	// This makes it so blueprints can modify the effect dynamically
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSettings(const FAudioVolumeScaleSettings& InSettings);

	// This is the thing which shows up in the content browser to let you expose the settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ShowOnlyInnerProperties))
		FAudioVolumeScaleSettings Settings;
};

// Flanger code here! 
USTRUCT(BlueprintType)
struct FFlangerSettings
{
	GENERATED_USTRUCT_BODY()

	// The LFO frequency (rate) that varies the delay time, in Hz
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (DisplayName="LFO Frequency", ClampMin = "0.0", ClampMax = "15.0", UIMin = "0.0", UIMax = "15.0"))
	float LFOFreq;

	// The LFO amplitude (strength) that scales the delay time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (DisplayName = "LFO Amplitude", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float LFOAmplitude;

	// The center delay amount (in milliseconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float CenterDelay;

	// Gain stage on dry (non-delayed signal)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float DryAmount;

	// Gain stage on wet (delayed) signal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float WetAmount;

	FFlangerSettings()
		: LFOFreq(0.5f)
		, LFOAmplitude(0.5f)
		, CenterDelay(0.5f)
		, DryAmount(1.0f)
		, WetAmount(1.0f)
	{}
};

// This is the instance of the effect. One is created for every source which is playing with the settings asset
class MYAUDIOPLUGIN_API FFlanger : public FSoundEffectSource
{
public:
	// Called on an audio effect at initialization on main thread before audio processing begins.
	virtual void Init(const FSoundEffectSourceInitData& InitData) override;

	// Called when an audio effect preset is changed
	virtual void OnPresetChanged() override;

	// Process the input block of audio. Called on audio render thread.
	virtual void ProcessAudio(const FSoundEffectSourceInputData& InData, float* OutAudioBufferData) override;

protected:
	// 2 Delay lines, one for each channel
	TArray<Audio::FDelay> Delays;
	FFlangerSettings SettingsCopy;
	AudioLibrary::Oscillator Osc;

};

UCLASS(ClassGroup = AudioSourceEffect, meta = (BlueprintSpawnableComponent))
class MYAUDIOPLUGIN_API UFlangerPreset : public USoundEffectSourcePreset
{
	GENERATED_BODY()

public:
	// This is a little helper macro to expose a bunch of functions easily
	EFFECT_PRESET_METHODS(Flanger)

		// Can override the preset color in the content browser if you want
		virtual FColor GetPresetColor() const override { return FColor(127.0f, 0.0f, 0.0f); }

	// This makes it so blueprints can modify the effect dynamically
	UFUNCTION(BlueprintCallable, Category = "Settings")
		void SetSettings(const FFlangerSettings& InSettings);

	// This is the thing which shows up in the content browser to let you expose the settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ShowOnlyInnerProperties))
		FFlangerSettings Settings;
};