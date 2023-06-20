// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API APotion : public AItem
{
	GENERATED_BODY()

protected:

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	float HealthPoints;

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	float StaminaPoints;

public:

	FORCEINLINE float GetHealthPoints() const { return HealthPoints; }
	FORCEINLINE float GetStaminaPoints() const { return StaminaPoints; }
};
