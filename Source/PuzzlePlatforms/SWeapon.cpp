// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp=CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
	RootComponent=MeshComp;
	MuzzleSocketName="MuzzleSocket";
	TracerTargetName="BeamEnd";

	//SetIsReplicatedByDefault(true);
	//SetReplicates(true);
	bReplicates=true;

	NetUpdateFrequency=66.f;
	MinNetUpdateFrequency=33.f;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBWShots=60/RateOfFire;
}

void ASWeapon::Fire()
{

	if(GetLocalRole()<ROLE_Authority){
		ServerFire();
		//return;
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner){
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection=EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection* 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial=true;

		FVector TracerEndPoint=TraceEnd;
		EPhysicalSurface SurfaceType=SurfaceType_Default;
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, 
		TraceEnd, ECC_GameTraceChannel1, QueryParams)){
			AActor* HitActor=Hit.GetActor();
			SurfaceType= UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage=BaseDamage;
			if(SurfaceType==SurfaceType2){
				ActualDamage*=2;
			}
			UGameplayStatics::ApplyPointDamage(HitActor,ActualDamage,
			ShotDirection,Hit,MyOwner->GetInstigatorController(),this,DamageType);
			
			PlayImpactEffects(SurfaceType,Hit.ImpactPoint);
			
			TracerEndPoint=Hit.ImpactPoint;
			
		}
		//DrawDebugLine(GetWorld(),EyeLocation,TraceEnd,FColor::White,false,1.0f,0,1.0f);

		PlayFireEffects(TracerEndPoint);
		
		if(GetLocalRole()==ROLE_Authority){
			HitScanTrace.TraceTo=TracerEndPoint;
			HitScanTrace.SurfaceType=SurfaceType;
		}

		LastFireTime=GetWorld()->TimeSeconds;
	}
}

void ASWeapon::ServerFire_Implementation(){
	Fire();
}

bool ASWeapon::ServerFire_Validate(){
	return true;
}

void ASWeapon::StartFire(){
	float FirstDelay=FMath::Max(LastFireTime+TimeBWShots-GetWorld()->TimeSeconds,0.f);
	GetWorldTimerManager().SetTimer(TH_TimeBWShots,this,&ASWeapon::Fire,TimeBWShots,true,FirstDelay);
}
void ASWeapon::StopFire(){
	GetWorldTimerManager().ClearTimer(TH_TimeBWShots);
}

void ASWeapon::PlayFireEffects(FVector TraceEnd){
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect,
											   MeshComp, MuzzleSocketName);
	}
	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent *TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType,FVector ImpactPoint){
	UParticleSystem *SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SurfaceType1:
		SelectedEffect = FleshImpactEffect;
		break;
	case SurfaceType2:
		SelectedEffect = FleshImpactEffect;
		break;

	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}
	if (SelectedEffect)
	{
		FVector MuzzleLocation=MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection=ImpactPoint-MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
				SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASWeapon::OnRep_HitScanTrace(){
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType,HitScanTrace.TraceTo);
}



// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASWeapon,HitScanTrace,COND_SkipOwner);
}

//ASWeapon
