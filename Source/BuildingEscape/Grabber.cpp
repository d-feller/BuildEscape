// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	Input = GetOwner()->FindComponentByClass<UInputComponent>();

	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("%s has no PhysicsHandle Component."), *GetOwner()->GetName());
	}

	if (!Input) {
		UE_LOG(LogTemp, Error, TEXT("%s has no Input Component."), *GetOwner()->GetName());
	}

	Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewLocation,
		OUT PlayerViewRotation
	);

	FVector LineTraceEnd = PlayerViewLocation + PlayerViewRotation.Vector() * Reach;

	DrawDebugLine(
		GetWorld(),
		PlayerViewLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0,
		0,
		5.f
	);
	
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	if (Hit.GetActor()) {
		UE_LOG(LogTemp, Warning, TEXT("%s is in reach"), *Hit.GetActor()->GetName());
	}
}

