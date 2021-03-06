// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define OUT

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

	FindAudioComponent();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() >= MassToTriggerPressurePlate)
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

	if (!AudioComponent) {
		return;
	}
	
	if (!OpenDoorSoundPlayed) {
		AudioComponent->Play();
		OpenDoorSoundPlayed = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, InitialRotation.Yaw, DeltaTime, 5);
	GetOwner()->SetActorRotation(CurrentRotation);

	if (!AudioComponent) {
		return;
	}
	
	if (OpenDoorSoundPlayed) {
		AudioComponent->Play();
		OpenDoorSoundPlayed = false;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors;

	if (!PressurePlate) {
		return TotalMass;
	}

	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	for (int32 i = 0; i < OverlappingActors.Num(); i++)
	{
		AActor* Actor = OverlappingActors[i];
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no AudioComponent!"), *GetOwner()->GetName());
	}
}

