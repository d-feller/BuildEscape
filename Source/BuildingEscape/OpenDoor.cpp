// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the %s component on it, but no PressurePlate set."), *GetOwner()->GetName(), *GetName());
	}

	InitialRotation = GetOwner()->GetActorRotation();
	CurrentRotation = InitialRotation;
	TargetRotation = GetOwner()->GetActorRotation().Add(0., TargetYaw, 0.);
	PressurePlateTrigger = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && PressurePlate->IsOverlappingActor(PressurePlateTrigger))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	} else if (GetWorld()->GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)
	{
		CloseDoor(DeltaTime);
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, TargetRotation.Yaw, DeltaTime, 1);
	GetOwner()->SetActorRotation(CurrentRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, InitialRotation.Yaw, DeltaTime, 5);
	GetOwner()->SetActorRotation(CurrentRotation);
}

