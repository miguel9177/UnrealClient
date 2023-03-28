// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponRaycast.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "CharacterHealth.h"

// Sets default values for this component's properties
UWeaponRaycast::UWeaponRaycast()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
    
	// ...
}


// Called when the game starts
void UWeaponRaycast::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponRaycast::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UCharacterHealth* UWeaponRaycast::FireRaycast()
{
    //// Get the player controller
    //APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    //if (!PlayerController) return;

    //// Get the player's viewpoint
    //FVector PlayerViewLocation;
    //FRotator PlayerViewRotation;
    //PlayerController->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

    //// Set the start and end points for the raycast
    //FVector Start = PlayerViewLocation;
    //FVector End = Start + PlayerViewRotation.Vector() * 10000.0f; // Set the raycast range to 10000 units

    //// Perform the raycast using the Visibility channel
    //FHitResult HitResult;
    //bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

    //// Draw a debug line to visualize the raycast
    //DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 3.0f);

    //// Check if the raycast hit something
    //if (bHit)
    //{
    //    // Handle the hit result, e.g., apply damage to the hit actor
    //    AActor* HitActor = HitResult.GetActor();
    //    if (HitActor)
    //    {
    //        // Apply damage to the hit actor
    //        UGameplayStatics::ApplyDamage(HitActor, amountOfDamageToGivePerShot, PlayerController, this, DamageType);
    //    }
    //}

    return nullptr;
}

