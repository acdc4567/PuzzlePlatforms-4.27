// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "PuzzlePlatformsGameMode.h"
#include "PuzzlePlatformsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "SHealthComponent.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "EngineUtils.h"

APuzzlePlatformsGameMode::APuzzlePlatformsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	GameStateClass=ASGameState::StaticClass();
	PlayerStateClass=ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick=true;
	PrimaryActorTick.TickInterval=1.f;
}



void APuzzlePlatformsGameMode::StartWave(){

	WaveCount++;
	NoOfBotsToSpawn=2*WaveCount;
	GetWorldTimerManager().SetTimer(TH_BotSpawner,this,&APuzzlePlatformsGameMode::SpawnBotTimerElapsed,1.f,true,0.f );
	SetWaveState(EWaveState::WaveInProgress);	

}

void APuzzlePlatformsGameMode::SpawnBotTimerElapsed(){
	SpawnNewBot();
	NoOfBotsToSpawn--;
	if(NoOfBotsToSpawn<=0){
		EndWave();
	}
}

void APuzzlePlatformsGameMode::StartPlay(){
	Super::StartPlay();
	PrepareForNextWave();
}

void APuzzlePlatformsGameMode::EndWave(){
	GetWorldTimerManager().ClearTimer(TH_BotSpawner);
	SetWaveState(EWaveState::WaitingForComplete);

	//PrepareForNextWave();
}

void APuzzlePlatformsGameMode::PrepareForNextWave(){
	GetWorldTimerManager().SetTimer(TH_NextWaveStart,this,&APuzzlePlatformsGameMode::StartWave, TimeBetweenWaves,false);
	SetWaveState(EWaveState::WaitingToStart);
	RespawnDeadPlayers();
}

void APuzzlePlatformsGameMode::CheckWaveState(){

	bool bIsPreparingForWave=GetWorldTimerManager().IsTimerActive(TH_NextWaveStart);
	if(NoOfBotsToSpawn>0||bIsPreparingForWave){
		return;
	}
	bool bIsAnyBotAlive=false;
	for (TActorIterator<APawn> It(GetWorld()); It; ++It){
		APawn* TestPawn=*It;
		if(TestPawn==nullptr||TestPawn->IsPawnControlled()){
			continue;
		}		
		USHealthComponent* HealthComp=Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if(ensure(HealthComp)&&HealthComp->GetHealth()>0.f){
			bIsAnyBotAlive=true;
			break;
		}

	}
	if(!bIsAnyBotAlive){
		SetWaveState(EWaveState::WaveComplete);

		PrepareForNextWave();

	}

}

void APuzzlePlatformsGameMode::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
	CheckWaveState();
	CheckAnyPlayerAlive();
}

void APuzzlePlatformsGameMode::CheckAnyPlayerAlive(){
	for(FConstPlayerControllerIterator It= GetWorld()->GetPlayerControllerIterator();It;++It){
		APlayerController* PC=It->Get();
		if(PC&&PC->GetPawn()){
			APawn* MyPawn=PC->GetPawn();
			USHealthComponent* HealthComp=Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if(ensure(HealthComp)&&HealthComp->GetHealth()>0.f){
				
				return;
			}
		}
	}
	GameOver();
}

void APuzzlePlatformsGameMode::GameOver(){
	EndWave();
	SetWaveState(EWaveState::GameOver);

}

void APuzzlePlatformsGameMode::SetWaveState(EWaveState NewState){
	ASGameState* GS=GetGameState<ASGameState>();
	if(ensureAlways(GS)){
		GS->SetWaveState(NewState);	
	}
}

void APuzzlePlatformsGameMode::RespawnDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController *PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

//APuzzlePlatformsGameMode
