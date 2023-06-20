// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potion.h"
#include "Interfaces/PickupInterface.h"

void APotion::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!ActorHasTag(FName("Health")) && !ActorHasTag(FName("Stamina")))
		return;

	if (OtherActor)
	{
		IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
		if (PickupInterface)
		{
			if (ActorHasTag(FName("Health")))
			{
				PickupInterface->IncrementHealth(this);
			}
			else
			{
				PickupInterface->IncrementStamina(this);
			}
			SpawnPickupSystem();
			SpawnPickupSound();
			Destroy();
		}
	}
}