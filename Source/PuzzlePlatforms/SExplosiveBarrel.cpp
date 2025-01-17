// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	HealthCompx=CreateDefaultSubobject<USHealthComponent>("HealthComp");
	HealthCompx->OnHealthChanged.AddDynamic(this,&ASExplosiveBarrel::OnHealthChanged);

	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent=MeshComp;

	RadialForceComp=CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius=250;
	RadialForceComp->bImpulseVelChange=true;
	RadialForceComp->bAutoActivate=false;
	RadialForceComp->bIgnoreOwningActor=true;

	ExplosionImpulse=400;
	SetReplicates(true);
	//bReplicates=true;
	SetReplicateMovement(true);


}

void ASExplosiveBarrel::OnRep_Exploded(){
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	MeshComp->SetMaterial(0, ExplodedMaterial);

}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* HealthComp,
	float Health,float HealthDelta,
	const class UDamageType* DamageType,
	class AController* InstigatedBy,AActor* DamageCauser){

	if (bExploded){
		return;
	}
	if (Health <= 0.f){
		bExploded = true;
		OnRep_Exploded();
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);
		RadialForceComp->FireImpulse();
	}
}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASExplosiveBarrel,bExploded);
	
}

//ASExplosiveBarrel
