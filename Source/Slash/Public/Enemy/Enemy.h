// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UENUM()
enum class EHitQuadrant : uint8
{
	EHQ_Front UMETA(DisplayName = " -45 <= Theta < 45"),
	EHQ_Back UMETA(DisplayName = " -135 < Theta <= 135"),
	EHQ_Left UMETA(DisplayName = " -135 <= Theta < -45"),
	EHQ_Right UMETA(DisplayName = " 45 <= Theta < 135")
};

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	void DirectionalHitReact();
	void DirectionalDeathReact();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	/**
	* Play montage functions
	*/
	void PlayHitReactMontage(const FName& SectionName);
	void Die(const FName& SectionName);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	bool InTargetRange(AActor* Target, double AcceptanceRadius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:
	/**
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticle;

	/**
	* Components
	*/
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	void SetDirectionalHitQuadrant(const FVector& ImpactPoint);

	EHitQuadrant DirectionalHitQuadrant = EHitQuadrant::EHQ_Front;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/** 
	* Navigation
	*/
	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	void CheckPatrolTarget();
	void CheckCombatTarget();

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 8.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float RunningSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WalkingSpeed = 125.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
