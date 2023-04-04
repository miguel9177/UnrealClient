// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "NetworkGameObject.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"

AGameManager* AGameManager::instance;

//constructor
AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
    //this initializes the instance variable, since this is a singleton
    instance = this;
}

//we do the get mandatory get instance code for a singleton
AGameManager* AGameManager::GetInstance()
{
    //if theres no instance we create a new one
    if (!instance)
    {
        instance = NewObject<AGameManager>();
        instance->AddToRoot();
    }

    return instance;
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();

    //this will store every actor in the world
    TArray<AActor*> allCharacterActors;
    //we get all the AActors that has a ACharacter
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), allCharacterActors);

    //if we did find an item
    if (allCharacterActors.Num() > 0)
    {
        //convert our array of AActor to ACharacter
        ACharacter* ourCharacter = Cast<ACharacter>(allCharacterActors[0]);

        //if we did find a character
        if (ourCharacter)
        {
            //we get the player controller from the our character
            ourPlayerController = Cast<APlayerController>(ourCharacter->GetController());
            //we get the network gameobject from our character
            ourPlayerNetworkGameObject = Cast<UNetworkGameObject>(ourCharacter->GetComponentByClass(UNetworkGameObject::StaticClass()));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No character instance found in the game world"));
    }
}

void AGameManager::PostInitializeComponents()
{
    Super::PostInitializeComponents(); 
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region Getters

//this returns our player network gameobject
UNetworkGameObject* AGameManager::GetOurPlayerNetworkGameObject()
{
    return ourPlayerNetworkGameObject;
}

//this returns our player controller
APlayerController* AGameManager::GetOurPlayerController()
{
    return ourPlayerController;
}

#pragma endregion

