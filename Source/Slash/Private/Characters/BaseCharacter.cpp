// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisonEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponHitBox())
	{
		EquippedWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::PlayAttackMontage()
{
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::Die(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(5.f);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
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

void ABaseCharacter::DirectionalDeathReact()
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
