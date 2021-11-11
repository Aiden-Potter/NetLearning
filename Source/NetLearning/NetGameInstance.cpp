// Fill out your copyright notice in the Description page of Project Settings.
#include "NetGameInstance.h"
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "Misc/NetworkVersion.h"


void UNetGameInstance::Init()
{
	Super::Init();
	// create a ReplayStreamer for FindReplays() and DeleteReplay(..)
    const TCHAR* TName = TEXT("LocalFileNetworkReplayStreaming");
    EnumerateStreamsPtr = FNetworkReplayStreaming::Get().GetFactory(TName).CreateReplayStreamer();
	
	// Link FindReplays() delegate to function
	OnEnumerateStreamsCompleteDelegate =
		FEnumerateStreamsCallback::CreateUObject(this,&UNetGameInstance::OnEnumerateStreamsComplete);

	// Link DeleteReplay() delegate to function
	OnDeleteFinishedStreamCompleteDelegate =
		FDeleteFinishedStreamCallback::CreateUObject(this, &UNetGameInstance::OnDeleteFinishedStreamComplete);

}

void UNetGameInstance::StartRecordingLocalFileReplay(FString ReplayName, FString FriendlyName)
{
	StartRecordingReplay(ReplayName, FriendlyName);
}

void UNetGameInstance::StopRecordingLocalFileReplay()
{
	StopRecordingReplay();
}
void UNetGameInstance::PlayLocalReplay(const FString& InName)
{
	Super::PlayReplay(InName);

}

void UNetGameInstance::FindReplays()
{
	if (EnumerateStreamsPtr.Get())
	{
		EnumerateStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(),
			0, FString(), TArray< FString >(), OnEnumerateStreamsCompleteDelegate);
	}
}

void UNetGameInstance::RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName)
{
	// Get File Info
	FNullReplayInfo Info;

	const FString DemoPath = FPaths::Combine(*FPaths::ProjectSavedDir(), TEXT("Demos/"));
	const FString StreamDirectory = FPaths::Combine(*DemoPath, *ReplayName);
	const FString StreamFullBaseFilename = FPaths::Combine(*StreamDirectory, *ReplayName);
	const FString InfoFilename = StreamFullBaseFilename + TEXT(".replayinfo");

	
	TUniquePtr<FArchive> InfoFileArchive(IFileManager::Get().CreateFileReader(*InfoFilename));

	if (InfoFileArchive.IsValid() && InfoFileArchive->TotalSize() != 0)
	{
		FString JsonString;
		*InfoFileArchive << JsonString;

		Info.FromJson(JsonString);
		Info.bIsValid = true;

		InfoFileArchive->Close();
	}

	// Set FriendlyName
	Info.FriendlyName = NewFriendlyReplayName;

	// Write File Info
	TUniquePtr<FArchive> ReplayInfoFileAr(IFileManager::Get().CreateFileWriter(*InfoFilename));

	if (ReplayInfoFileAr.IsValid())
	{
		FString JsonString = Info.ToJson();
		*ReplayInfoFileAr << JsonString;

		ReplayInfoFileAr->Close();
	}
}

void UNetGameInstance::DeleteReplay(const FString& ReplayName)
{
	if (EnumerateStreamsPtr.Get())
	{
		EnumerateStreamsPtr.Get()->DeleteFinishedStream(ReplayName, 0, OnDeleteFinishedStreamCompleteDelegate);
	}
}

void UNetGameInstance::OnEnumerateStreamsComplete(const FEnumerateStreamsResult& Result)
{
	TArray<FS_ReplayInfo> AllReplays;
 
	for (FNetworkReplayStreamInfo StreamInfo : Result.FoundStreams)
	{
		if (!StreamInfo.bIsLive)
		{
			AllReplays.Add(FS_ReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
		}
	}
	OnFindReplaysComplete.Broadcast(AllReplays);
}

void UNetGameInstance::OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& Result)
{
	FindReplays();
}
