// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkReplayStreaming.h"
#include "Engine/GameInstance.h"
#include "NetGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FS_ReplayInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ReplayName;

	UPROPERTY(BlueprintReadOnly)
	FString FriendlyName;

	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadOnly)
	int32 LengthInMS;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid;

	FS_ReplayInfo(FString NewName, FString NewFriendlyName, FDateTime NewTimestamp, int32 NewLengthInMS)
	{
		ReplayName = NewName;
		FriendlyName = NewFriendlyName;
		Timestamp = NewTimestamp;
		LengthInMS = NewLengthInMS;
		bIsValid = true;
	}

	FS_ReplayInfo()
	{
		ReplayName = "Replay";
		FriendlyName = "Replay";
		Timestamp = FDateTime::MinValue();
		LengthInMS = 0;
		bIsValid = false;
	}
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindReplaysComplete, const TArray<FS_ReplayInfo> &, AllReplays);
/**
 * 
 */
UCLASS()
class NETLEARNING_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	UFUNCTION(BlueprintCallable,Category = "Replays")
	void StartRecordingLocalFileReplay(FString ReplayName, FString FriendlyName);
	UFUNCTION(BlueprintCallable,Category = "Replays")
	void StopRecordingLocalFileReplay();

	UFUNCTION(BlueprintCallable,Category = "Replays")
	void PlayLocalReplay(const FString& InName);
	/** Start looking for/finding replays on the hard drive */
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void FindReplays();

	/** Apply a new custom name to the replay (for UI only) */
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void RenameReplay(const FString &ReplayName, const FString &NewFriendlyReplayName);

	/** Delete a previously recorded replay */
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void DeleteReplay(const FString &ReplayName);

protected:
	UPROPERTY(BlueprintAssignable)
	FOnFindReplaysComplete OnFindReplaysComplete;
private:

	// for FindReplays() 
	TSharedPtr<INetworkReplayStreamer> EnumerateStreamsPtr;
	FEnumerateStreamsCallback OnEnumerateStreamsCompleteDelegate;

	void OnEnumerateStreamsComplete(const FEnumerateStreamsResult& Result);

	// for DeleteReplays(..)
	FDeleteFinishedStreamCallback OnDeleteFinishedStreamCompleteDelegate;
	void OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& Result);
};
