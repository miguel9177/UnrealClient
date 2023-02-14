// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameObject.h"
#include "NetManager.h"
int32 UNetworkGameObject::lastLocalID = 0;

// Sets default values for this component's properties
UNetworkGameObject::UNetworkGameObject()
{
	isLocallyOwned = false;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// ...
}


// Called when the game starts
void UNetworkGameObject::BeginPlay()
{
	Super::BeginPlay();
	if (isLocallyOwned) {
		localId = lastLocalID;
		lastLocalID++;
		ANetManager::localNetObjects.Add(this);
		UE_LOG(LogTemp, Warning, TEXT("count: %d"), ANetManager::localNetObjects.Num());
	}	
}


// Called every frame
void UNetworkGameObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UNetworkGameObject::GetGlobalID() {
	return globalId;
}

int32 UNetworkGameObject::GetLocalID() {
	return localId;
}

void UNetworkGameObject::SetGlobalID(int32 gid) {
	globalId = gid;
}

void UNetworkGameObject::SetLocalID(int32 lid) {
	localId = lid;
}


