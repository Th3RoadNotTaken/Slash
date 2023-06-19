// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if(Hitter)
		SetDirectionalHitQuadrant(Hitter->GetActorLocation());
	if (IsAlive())
	{
		DirectionalHitReact();
	}
	else
	{
		Die();
	}
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::SetDirectionalHitQuadrant(const FVector& ImpactPoint)
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

void ABaseCharacter::DirectionalHitReact()
{
	FName HitReactMontageSection("ReactFromBack");

	switch (DirectionalHitQuadrant)
	{
	case EHitQuadrant::EHQ_Front:
		HitReactMontageSection = FName("ReactFromFront");
		break;
	case EHitQuadrant::EHQ_Left:
		HitReactMontageSection = FName("ReactFromLeft");
		break;
	case EHitQuadrant::EHQ_Right:
		HitReactMontageSection = FName("ReactFromRight");
		break;
	default:
		HitReactMontageSection = FName("ReactFromBack");
		break;
	}
	PlayHitReactMontage(HitReactMontageSection);
}

void ABaseCharacter::DirectionalDeathReact()
{
	FName DeathMontageSection("DeathFromFront1");
	DeathPose = EDeathPose::EDP_Death2;
	const int32 DeathFromFrontSelection = FMath::RandRange(0, 1);

	switch (DirectionalHitQuadrant)
	{
	case EHitQuadrant::EHQ_Front:
		if (DeathFromFrontSelection == 1)
		{
			DeathMontageSection = FName("DeathFromFront2");
			DeathPose = EDeathPose::EDP_Death3;
		}
		break;
	case EHitQuadrant::EHQ_Left:
		DeathMontageSection = FName("DeathFromLeft");
		DeathPose = EDeathPose::EDP_Death4;
		break;
	case EHitQuadrant::EHQ_Right:
		DeathMontageSection = FName("DeathFromRight");
		DeathPose = EDeathPose::EDP_Death5;
		break;
	case EHitQuadrant::EHQ_Back:
		DeathMontageSection = FName("DeathFromBack");
		DeathPose = EDeathPose::EDP_Death1;
		break;
	default:
		break;
	}
	PlayMontageSection(DeathMontage, DeathMontageSection);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMesh()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::SetWeaponCollisonEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponHitBox())
	{
		EquippedWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::PlayAttackMontage(const TArray<FName>& AttackMontageSections)
{
	if (AttackMontageSections.Num() <= 0) return;
	const int32 MaxSectionIndex = AttackMontageSections.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(AttackMontage, AttackMontageSections[Selection]);
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, TEXT("Default"));
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	PlayMontageSection(HitReactMontage, SectionName);
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	DirectionalDeathReact();
	DisableCapsule();
	DisableMesh();
	SetWeaponCollisonEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr)
		return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}
