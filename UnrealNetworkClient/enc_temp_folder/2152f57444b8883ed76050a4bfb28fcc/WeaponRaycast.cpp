// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponRaycast.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "NetworkGameObject.h"
#include "NetManager.h"
#include "GameManager.h"
#include "GameFramework/Character.h"

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

    if(AGameManager::GetInstance()->GetOurPlayerController() != nullptr)
        SetupInputs(AGameManager::GetInstance()->GetOurPlayerController()->InputComponent);
}

void UWeaponRaycast::SetupInputs(UInputComponent* playerInput)
{
    if (playerInput)
        playerInput->BindAction("Fire", IE_Pressed, this, &UWeaponRaycast::FireButtonClicked);
}

void UWeaponRaycast::FireButtonClicked()
{
 /*   UNetworkGameObject* characterWeHit = FireRaycast();

    if (characterWeHit == nullptr)
        return;

    ANetManager::singleton->AnEnemyPlayerWasShotByUs(characterWeHit, nameOfWeapon);*/
    // Get all actors with the UNetworkGameObject component

    TArray<AActor*> FoundActors;
    TArray<UNetworkGameObject*> networkGameObjects;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        UNetworkGameObject* NetworkGameObject = Cast<UNetworkGameObject>(Actor->GetComponentByClass(UNetworkGameObject::StaticClass()));
        if (NetworkGameObject)
        {
            networkGameObjects.Add(NetworkGameObject);
        }
    }

    if (networkGameObjects.Num() > 1)
    {
        ANetManager::singleton->AnEnemyPlayerWasShotByUs(networkGameObjects[1], nameOfWeapon);
    }
}

// Called every frame
void UWeaponRaycast::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UNetworkGameObject* UWeaponRaycast::FireRaycast()
{
    if (!AGameManager::GetInstance()->GetOurPlayerController()) 
        return nullptr;

    //this gets the player vieport
    FVector playerPos;
    FRotator playerRot;
    AGameManager::GetInstance()->GetOurPlayerController()->GetPlayerViewPoint(playerPos, playerRot);

    //get a vector at the end of raycast, in this case, character look duration *1000 of distance
    FVector raycastEndPos = playerPos + playerRot.Vector() * 10000.0f; 

    //this will get the hit result
    FHitResult hit;
    bool hasHit = GetWorld()->LineTraceSingleByChannel(hit, playerPos, raycastEndPos, ECollisionChannel::ECC_Visibility);

    //draws a line for debugging purposes
    DrawDebugLine(GetWorld(), playerPos, raycastEndPos, FColor::Red, false, 2.0f, 0, 3.0f);

    //if we didnt hit anything, we leave
    if (!hasHit)
        return nullptr;

    //this will get the charcter that it hit
    AActor* hitActor = hit.GetActor();
    //if no character we leave
    if (!hitActor)
        return nullptr;
        
    //we get the character network game object script from the actor we hit
    UNetworkGameObject* hitNetworkGameObjectComponent = Cast<UNetworkGameObject>(hitActor->GetComponentByClass(UNetworkGameObject::StaticClass()));;
    //if theres an hit script we reduce its life
    if (hitNetworkGameObjectComponent)
        return hitNetworkGameObjectComponent;

    return nullptr;
}

