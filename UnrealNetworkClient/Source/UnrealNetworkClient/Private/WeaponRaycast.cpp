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

    //this gets the player controller, so that i can access its vieport
    myPlayerController = GetWorld()->GetFirstPlayerController();

    if (myPlayerController)
        SetupInputs(myPlayerController->InputComponent);
}

void UWeaponRaycast::SetupInputs(UInputComponent* playerInput)
{
    if (playerInput)
        playerInput->BindAction("Fire", IE_Pressed, this, &UWeaponRaycast::FireButtonClicked);
    
}

void UWeaponRaycast::FireButtonClicked()
{
    UCharacterHealth* characterHealthHit = FireRaycast();

    if (characterHealthHit == nullptr)
        return;

    characterHealthHit->TakeDamage(amountOfDamageToGivePerShot);
}

// Called every frame
void UWeaponRaycast::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UCharacterHealth* UWeaponRaycast::FireRaycast()
{
    if (!myPlayerController) 
        return nullptr;

    //this gets the player vieport
    FVector playerPos;
    FRotator playerRot;
    myPlayerController->GetPlayerViewPoint(playerPos, playerRot);

    //get a vector at the end of raycast, in this case, character look duration *1000 of distance
    FVector raycastEndPos = playerPos + playerRot.Vector() * 10000.0f; 

    //this will get the hit result
    FHitResult hit;
    bool hasHit = GetWorld()->LineTraceSingleByChannel(hit, playerPos, raycastEndPos, ECollisionChannel::ECC_Visibility);

    //draws a line for debugging purposes
    DrawDebugLine(GetWorld(), playerPos, raycastEndPos, FColor::Red, false, 2.0f, 0, 3.0f);

    // Check if the raycast hit something
    if (!hasHit)
        return nullptr;

    //this will get the charcter that it hit
    AActor* hitActor = hit.GetActor();
    if (!hitActor)
        return nullptr;
        
    UCharacterHealth* hitHealthComponent = Cast<UCharacterHealth>(hitActor->GetComponentByClass(UCharacterHealth::StaticClass()));;    
    if (hitHealthComponent)
        return hitHealthComponent;

    return nullptr;
}

