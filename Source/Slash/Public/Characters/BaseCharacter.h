// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UHealthBarComponent;
class UAttributeComponent;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisonEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;

	virtual void Attack();

	/**
	* Play montage functions
	*/
	void PlayAttackMontage(const TArray<FName>& AttackMontageSections);
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void Die(const FName& SectionName);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void DisableCapsule();

	virtual bool CanAttack();
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	void SetDirectionalHitQuadrant(const FVector& ImpactPoint);

	EHitQuadrant DirectionalHitQuadrant = EHitQuadrant::EHQ_Front;

	void DirectionalHitReact();
	void DirectionalDeathReact();

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	/**
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	/** 
	*Components
	*/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

private:

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticle;
};
