// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar Widget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AEnemy::InTargetRange(AActor* Target, double AcceptanceRadius)
{
	if (Target == nullptr)
		return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	DRAW_SPHERE_SINGLEFRAME(GetActorLocation());
	DRAW_SPHERE_SINGLEFRAME(Target->GetActorLocation());
	return DistanceToTarget <= AcceptanceRadius;
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
	SetDirectionalHitQuadrant(ImpactPoint);
	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact();
	}
	else
	{
		DirectionalDeathReact();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}

void AEnemy::SetDirectionalHitQuadrant(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal(); // Gets the vector from the impact point to the enemy's location and normalizes it

	// Forward * ToHit = |Forward| * |ToHit| * cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta); // Inverse of cosine. Gives angle in radians
	Theta = FMath::RadiansToDegrees(Theta);

	// If CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	
	if (Theta >= -45.f && Theta < 45.f)
	{
		DirectionalHitQuadrant = EHitQuadrant::EHQ_Front;
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		DirectionalHitQuadrant = EHitQuadrant::EHQ_Left;
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		DirectionalHitQuadrant = EHitQuadrant::EHQ_Right;
	}
	else
	{
		DirectionalHitQuadrant = EHitQuadrant::EHQ_Back;
	}
}

void AEnemy::DirectionalHitReact()
{
	FName HitReactMontageSection("ReactFromBack");

	if (DirectionalHitQuadrant == EHitQuadrant::EHQ_Front)
	{
		HitReactMontageSection = FName("ReactFromFront");
	}
	else if (DirectionalHitQuadrant == EHitQuadrant::EHQ_Left)
	{
		HitReactMontageSection = FName("ReactFromLeft");
	}
	else if (DirectionalHitQuadrant == EHitQuadrant::EHQ_Right)
	{
		HitReactMontageSection = FName("ReactFromRight");
	}
	else
	{
		HitReactMontageSection = FName("ReactFromBack");
	}

	PlayHitReactMontage(HitReactMontageSection);
}

void AEnemy::DirectionalDeathReact()
{
	FName DeathMontageSection("DeathFromFront1");
	DeathPose = EDeathPose::EDP_Death2;
	const int32 DeathFromFrontSelection = FMath::RandRange(0, 1);

	if (DirectionalHitQuadrant == EHitQuadrant::EHQ_Front)
	{
		if (DeathFromFrontSelection == 1)
		{
			DeathMontageSection = FName("DeathFromFront2");
			DeathPose = EDeathPose::EDP_Death3;
		}
	}
	else if (DirectionalHitQuadrant == EHitQuadrant::EHQ_Left)
	{
		DeathMontageSection = FName("DeathFromLeft");
		DeathPose = EDeathPose::EDP_Death4;
	}
	else if (DirectionalHitQuadrant == EHitQuadrant::EHQ_Right)
	{
		DeathMontageSection = FName("DeathFromRight");
		DeathPose = EDeathPose::EDP_Death5;
	}
	else
	{
		DeathMontageSection = FName("DeathFromBack");
		DeathPose = EDeathPose::EDP_Death1;
	}

	Die(DeathMontageSection);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Die(const FName& SectionName)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);

		if (HealthBarWidget)
		{
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		}
	}

	CombatTarget = EventInstigator->GetPawn();

	return DamageAmount;
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// Outside combat radius, lose interest
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
		MoveToTarget(PatrolTarget);
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const int32 RandWaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, RandWaitTime);
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr)
		return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);
	FNavPathSharedPtr NavPath;

	EnemyController->MoveTo(MoveRequest, &NavPath);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 PatrolTargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[PatrolTargetSelection];
	}
	return nullptr;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Chasing)
		return;
	if (SeenPawn && SeenPawn->ActorHasTag(FName("SlashCharacter")))
	{
		EnemyState = EEnemyState::EES_Chasing;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		CombatTarget = SeenPawn;
		MoveToTarget(CombatTarget);
	}
}