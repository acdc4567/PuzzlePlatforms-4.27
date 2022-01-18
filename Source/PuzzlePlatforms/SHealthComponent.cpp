// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "PuzzlePlatformsGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority){
		AActor *MyOwner = GetOwner();
		if (MyOwner){
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health=DefaultHealth;
}
void USHealthComponent::HandleTakeAnyDamage(
 AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
  class AController* InstigatedBy, AActor* DamageCauser){
	if (Damage <= 0||bIsDead){
		return;
	}

	if(DamageCauser!=DamagedActor&& IsFriendly(DamagedActor,DamageCauser)){
		return;
	}

	Health=FMath::Clamp(Health-Damage,0.f,DefaultHealth);
	UE_LOG(LogTemp,Warning,TEXT("HealthChanged:%s"),*FString::SanitizeFloat(Health));

	bIsDead=Health<=0.f;
	OnHealthChanged.Broadcast(this,Health,Damage,DamageType,InstigatedBy,DamageCauser);
		//ClientOnHealthChanged(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)	
	{
		APuzzlePlatformsGameMode *GM = Cast<APuzzlePlatformsGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser,InstigatedBy);
		}
	}
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::ClientOnHealthChanged_Implementation(USHealthComponent* HealthComponent, float SHealth, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
   OnHealthChanged.Broadcast(this, SHealth, HealthDelta, DamageType, InstigatedBy, DamageCauser);
}


void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHealthComponent,Health);
}
void USHealthComponent::Heal(float HealAmount){
	if(HealAmount<=0.f||Health<=0.f) return;

	Health=FMath::Clamp(Health+HealAmount,0.f,DefaultHealth);
	OnHealthChanged.Broadcast(this,Health,-HealAmount,nullptr,nullptr,nullptr);

}

bool USHealthComponent::IsFriendly(AActor* ActorA,AActor* ActorB){
	if(ActorA==nullptr||ActorB==nullptr){
		return true;
	}
	
	USHealthComponent* HealthCompA=Cast<USHealthComponent> (ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB=Cast<USHealthComponent> (ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	if(HealthCompA==nullptr||HealthCompB==nullptr){
		return true;
	}
	return HealthCompA->TeamNum==HealthCompB->TeamNum;

}


//USHealthComponent
