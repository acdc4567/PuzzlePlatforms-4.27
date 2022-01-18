// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SPowerupActor.h"
#include "TimerManager.h"

// Sets default values
ASPickupActor::ASPickupActor()
{
 	SphereComp=CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetSphereRadius(75.f);

	RootComponent=SphereComp;


	DecalComp=CreateDefaultSubobject<UDecalComponent>("DecalComp");
	DecalComp->SetRelativeRotation(FRotator(90.f,0.f,0.f));
	DecalComp->DecalSize=FVector(64.f,75.f,75.f);
	
	DecalComp->SetupAttachment(SphereComp);

	bReplicates=true;



}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if(GetLocalRole()==ROLE_Authority){
		Respawn();
	}
	
	
}
void ASPickupActor::NotifyActorBeginOverlap(AActor* OtherActor){
	Super::NotifyActorBeginOverlap(OtherActor);
	if (GetLocalRole()==ROLE_Authority&&PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerUp(OtherActor);
		PowerUpInstance = nullptr;
		GetWorldTimerManager().SetTimer(TH_RespawnTimer, this, &ASPickupActor::Respawn, CoolDownDuration);
	}
}

void ASPickupActor::Respawn(){
	if(PowerUpClass==nullptr){
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance= GetWorld()->SpawnActor<ASPowerupActor>(PowerUpClass,GetTransform(),SpawnParams);
}




//ASPickupActor
