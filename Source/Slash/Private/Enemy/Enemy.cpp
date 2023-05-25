// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

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
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget)
	{
		const double DistanceToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (DistanceToTarget > CombatRadius)
		{
			CombatTarget = nullptr;
			if (HealthBarWidget)
			{
				HealthBarWidget->SetVisibility(false);
			}
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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