//
// Copyright (C) Valve Corporation. All rights reserved.
//

#pragma once

#include "SteamAudioModule.h"
#include "Async/Async.h"

// ---------------------------------------------------------------------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------------------------------------------------------------------

namespace SteamAudio {

/** Converts a distance from Steam Audio units to Unreal units. */
float STEAMAUDIO_API ConvertSteamAudioDistanceToUnreal(float Distance);

/** Converts a 3D vector from Unreal's coordinate system to Steam Audio's coordinate system. */
IPLVector3 STEAMAUDIO_API ConvertVector(const FVector& UnrealCoords, bool bScale = true);

/** Converts a 3D vector from Steam Audio's coordinate system to Unreal's coordinate system. */
FVector STEAMAUDIO_API ConvertVectorInverse(const IPLVector3& SteamAudioCoords, bool bScale = true);

/** Converts a transform from Unreal's coordinate system to a 4x4 matrix in Steam Audio's coordinate system. */
IPLMatrix4x4 STEAMAUDIO_API ConvertTransform(const FTransform& UnrealTransform, bool bRowMajor = true, bool bScale = true);

/** Returns the IR size (in samples) corresponding to the given duration (in seconds). */
int STEAMAUDIO_API CalcIRSizeForDuration(float Duration, int SamplingRate);

/** Returns the number of channels corresponding to the given Ambisonic order. */
int STEAMAUDIO_API CalcNumChannelsForAmbisonicOrder(int Order);

/** Returns the speaker layout corresponding to the given number of channels. */
IPLSpeakerLayout STEAMAUDIO_API GetSpeakerLayoutForNumChannels(int NumChannels);

/** Returns the number of threads corresponding to the given CPU cores percentage. */
int STEAMAUDIO_API GetNumThreadsForCPUCoresPercentage(float Percentage);

/** Runs the given function on the game thread. */
template <typename ReturnType>
inline ReturnType RunInGameThread(TUniqueFunction<ReturnType()> Function)
{
    TPromise<ReturnType> Promise;

    AsyncTask(ENamedThreads::GameThread, [&]()
    {
        Promise.SetValue(Function());
    });

    TFuture<ReturnType> Future = Promise.GetFuture();
    Future.Wait();

    return Future.Get();
}

template <>
inline void RunInGameThread(TUniqueFunction<void()> Function)
{
    TPromise<void> Promise;

    AsyncTask(ENamedThreads::GameThread, [&]()
    {
        Function();
        Promise.SetValue();
    });

    TFuture<void> Future = Promise.GetFuture();
    Future.Wait();
}

}
