// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameObject.h"
#include "NetManager.h"
#include "CharacterHealth.h"

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
	//since we are a new network gameobject we add ourselfs to the net manager
	ANetManager::singleton->AddNetObject(this); 
	Super::BeginPlay();

	//if we are locally owned we store the local id and increase the static last local id variable, wich is a counter for use to be able to have unique local ids
	if (isLocallyOwned) {
		localId = lastLocalID;
		lastLocalID++;
		
	}	

	//if we can get our owner
	if (GetOwner())
	{
		//we get tge cgaarcter health script (brother sccript)
		characterHpScript = GetOwner()->FindComponentByClass<UCharacterHealth>();
		//if the character scrript is null we write the error
		if (!characterHpScript)
		{
			UE_LOG(LogTemp, Warning, TEXT("UCharacterHealth was not found!"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("count: %d"), ANetManager::localNetObjects.Num());
}


// Called every frame
void UNetworkGameObject::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//this returns if we are locally owned or not
bool UNetworkGameObject::GetIsLocallyOwned()
{
	return isLocallyOwned;
}

//this returns the global id
int32 UNetworkGameObject::GetGlobalID() {
	return globalId;
}

//this returns the local id
int32 UNetworkGameObject::GetLocalID() {
	return localId;
}

//this sets the gloabal id
void UNetworkGameObject::SetGlobalID(int32 gid) {
	globalId = gid;
}

//this sets the local id
void UNetworkGameObject::SetLocalID(int32 lid) {
	localId = lid;
}

//this transforms our infomation into a string, so that we can send the information to the server
FString UNetworkGameObject::ToPacket() {
	//we get our actor
	AActor* parentActor = GetOwner();
	FVector position = parentActor->GetActorLocation();//we store the position of the actor
	FRotator rotation = parentActor->GetActorRotation(); //we get the actor rotation (euler)
	FQuat quaternionRotation = FQuat(rotation); //we convert the rot to a quaternion
	//we create a string with all character info
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

//this receives a packet and returns the global id
int32 UNetworkGameObject::GlobalIDFromPacket(FString packet) {
	TArray<FString> parsed;
	packet.ParseIntoArray(parsed, TEXT(";"), false);
	return FCString::Atoi(*parsed[1]);
}

//this receives a packet and edits this actor settings
void UNetworkGameObject::FromPacket(FString packet) 
{ 
	//we replace the , with . to fix the problems my pc had since its an portuguese pc
	packet = packet.Replace(TEXT(","), TEXT("."));
	TArray<FString> parsed;//this stores the parsed string
	packet.ParseIntoArray(parsed, TEXT(";"), false);//we parce the string
	//if the parsed values is 10 
	if (parsed.Num() == 10)
	{
		//*parsed[3].Replace(TEXT(","), TEXT("."));
		AActor* parentActor = GetOwner();//this gets the actor
		FVector position = FVector(FCString::Atof(*parsed[2]), FCString::Atof(*parsed[3]), FCString::Atof(*parsed[4]));//gets the position from the packet
		FQuat rotation = FQuat(FCString::Atof(*parsed[5]), FCString::Atof(*parsed[6]), FCString::Atof(*parsed[7]), FCString::Atof(*parsed[8]));//gets the rotation from the packet
		parentActor->SetActorLocation(position);//this sets the actor position 
		parentActor->SetActorRotation(rotation);//this sets the actor rotation
		hp = FCString::Atof(*parsed[9]);//this stores the hp
		characterHpScript->SetAmountOfHealth(hp);//this edits the hp of the character
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("bad packet data recieved"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Bad Packet Data REceived: ");
	}
}

//this updates the hp from the packet
void UNetworkGameObject::UpdateHpFromPacket(FString packet)
{
	//we replace the , with . to fix the problems my pc had since its an portuguese pc
	packet = packet.Replace(TEXT(","), TEXT("."));
	TArray<FString> parsed;//this stores the parsed string
	packet.ParseIntoArray(parsed, TEXT(";"), false);//we parce the string

	//if the string has 10 items 
	if (parsed.Num() == 10)
	{
		//*parsed[3].Replace(TEXT(","), TEXT("."));
		hp = FCString::Atof(*parsed[9]);//we store the hp
		characterHpScript->SetAmountOfHealth(hp);//this sets the hp of the player
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("bad packet data recieved"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Bad Packet Data REceived: ");
	}
}


