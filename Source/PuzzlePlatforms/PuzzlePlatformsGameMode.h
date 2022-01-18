// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "PuzzlePlatformsGameMode.generated.h"

enum class EWaveState:uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor *, VictimActor, AActor *, KillerActor,AController* , KillerController);

UCLASS(minimalapi)
class APuzzlePlatformsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APuzzlePlatformsGameMode();

protected:
	UFUNCTION(BlueprintImplementableEvent,Category=GameMode)
	void SpawnNewBot();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void SpawnBotTimerElapsed();

	FTimerHandle TH_BotSpawner;

	FTimerHandle TH_NextWaveStart;

	int32 NoOfBotsToSpawn;

	int32 WaveCount=0;

	UPROPERTY(EditDefaultsOnly,Category=GameMode)
	float TimeBetweenWaves=2.f;

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RespawnDeadPlayers();


public:

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable,Category=Score)
	FOnActorKilled OnActorKilled;

};



