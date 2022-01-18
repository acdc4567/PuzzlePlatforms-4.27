// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	bReplicates=true;

}

void ASPowerupActor::OnTickPowerUp(){
	++TicksProcessed;
	OnPowerUpTicked();
	if (TicksProcessed >=  TotalNoOfTicks)
	{
		OnExpired();

		bIsPowerUpActive=false;
		OnRep_PowerUpActive();
		GetWorldTimerManager().ClearTimer(TH_PowerUpTick);
	}
}

void ASPowerupActor::ActivatePowerUp(AActor* OtherActorx){
	OnActivated(OtherActorx);
	bIsPowerUpActive=true;
	OnRep_PowerUpActive();
	if(PowerUpInterval>0.f){
		GetWorldTimerManager().SetTimer(TH_PowerUpTick,this,&ASPowerupActor::OnTickPowerUp,PowerUpInterval,true);
		
	}
	else
	{
		OnTickPowerUp();
	}

}
void ASPowerupActor::OnRep_PowerUpActive(){
	OnPowerUpStateChanged( bIsPowerUpActive);
}


void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPowerupActor,bIsPowerUpActive);
}


//ASPowerupActor
