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
	ANetManager::singleton->AddNetObject(this); //calls the above method on the singleton network manager
	Super::BeginPlay();
	if (isLocallyOwned) {
		localId = lastLocalID;
		lastLocalID++;
		
	}	


	UE_LOG(LogTemp, Warning, TEXT("count: %d"), ANetManager::localNetObjects.Num());
}


// Called every frame
void UNetworkGameObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UNetworkGameObject::GetIsLocallyOwned()
{
	return isLocallyOwned;
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

FString UNetworkGameObject::ToPacket() {
	AActor* parentActor = GetOwner();
	FVector position = parentActor->GetActorLocation();
	FRotator rotation = parentActor->GetActorRotation(); //unreal uses euler angles..
	FQuat quaternionRotation = FQuat(rotation); //so we have to convert to Quaternion for Unity consistence

	FString returnVal = FString::Printf(TEXT("Object data;%i;%f;%f;%f;%f;%f;%f;%f"), globalId,
		position.X,
		position.Y,
		position.Z,
		quaternionRotation.X,
		quaternionRotation.Y,
		quaternionRotation.Z,
		quaternionRotation.W
	);
	return returnVal;
}

int32 UNetworkGameObject::GlobalIDFromPacket(FString packet) {
	TArray<FString> parsed;
	packet.ParseIntoArray(parsed, TEXT(";"), false);
	return FCString::Atoi(*parsed[1]);
}

void UNetworkGameObject::FromPacket(FString packet) { //returns global id
	TArray<FString> parsed;
	packet.ParseIntoArray(parsed, TEXT(";"), false);
	//*parsed[2].Replace(TEXT(","), TEXT("."));
	//*parsed[3].Replace(TEXT(","), TEXT("."));
	AActor* parentActor = GetOwner();
	FVector position = FVector(FCString::Atof(*parsed[2]), FCString::Atof(*parsed[3]), FCString::Atof(*parsed[4]));
	FQuat rotation = FQuat(FCString::Atof(*parsed[5]), FCString::Atof(*parsed[6]), FCString::Atof(*parsed[7]), FCString::Atof(*parsed[8]));
	parentActor->SetActorLocation(position);
	parentActor->SetActorRotation(rotation);

}


