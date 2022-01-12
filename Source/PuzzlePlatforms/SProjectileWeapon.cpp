// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileWeapon.h"
ASProjectileWeapon::ASProjectileWeapon(){
    bReplicates=true;
}
void ASProjectileWeapon::Fire() {
    AActor* MyOwner = GetOwner();
	if (MyOwner){
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

        FVector MuzzleLocation=MeshComp->GetSocketLocation(MuzzleSocketName);
        //FRotator MuzzleRotation=MeshComp->GetSocketRotation(MuzzleSocketName);

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        GetWorld()->SpawnActor<AActor>(ProjectileClass,MuzzleLocation,EyeRotation);
    }

}