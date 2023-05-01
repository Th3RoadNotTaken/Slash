// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/StaticMeshComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	AddActorWorldRotation(FRotator(30.f, 0.f, 0.f) * DeltaTime);
}

float AItem::TransformedSine()
{
	return Amplitude * FMath::Sin(TimeConstant * RunningTime);
}

float AItem::TransformedCosine()
{
	return Amplitude * FMath::Cos(TimeConstant * RunningTime);
}
