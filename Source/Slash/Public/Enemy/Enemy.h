// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	bool InTargetRange(AActor* Target, double AcceptanceRadius);
	void MoveToTarget(AActor* Target);
	virtual bool CanAttack() override;
	AActor* ChoosePatrolTarget();
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	virtual void Attack() override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	virtual void HandleDamage(float DamageAmount) override;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

private:
	/**
	* Components
	*/
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

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

	/**
	* AI Behaviour
	*/
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void StartChasing();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();

	/**
	* Combat
	*/
	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 0.8f;

	void StartAttackTimer();
	void ClearAttackTimer();
};
