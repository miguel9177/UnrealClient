// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h" //networking module – read above if this causes compile errors
#include "NetworkGameObject.h"
#include "NetManager.generated.h"

UCLASS()
class ANetManager : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> OtherPlayerAvatars;

public:
	ANetManager();
	~ANetManager();

	static TArray<UNetworkGameObject*> localNetObjects;
	static float timePastSinceBeginPlay;

	//notice the similarity in the types with the C# implementation. Primarily, we need a socket and endpoints, and arrays for data
	FSocket* Socket;

	// Local Endpoint
	FString SocketDescription = "UDP Listen Socket";
	FIPv4Endpoint LocalEndpoint;
	uint16 LocalPort = 0; //so the port is automatically assigned
	int32 SendSize;
	TArray<uint8> ReceivedData;

	// Remote Endpoint
	FIPv4Endpoint RemoteEndpoint;
	FIPv4Address RemoteAddress;
	uint16 RemotePort = 9050;
	int32 BufferSize;
	FString IP = "127.0.0.1";

	ISocketSubsystem* SocketSubsystem;

	//the methods we’ll use are similar to Unity’s Start (BeginPlay()) and Update (Tick()). We’ll also declare a method to listen to the socket for inbound data.

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	static ANetManager* singleton; //declare the class to contain a pointer to an instance of itself

	virtual void Tick(float DeltaTime) override;

	void Listen();

	//we’ll think about blueprint integration later, but this is the starting bit
	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		bool sendMessage(FString Message);

	void messageQueue();

	void AddNetObject(UNetworkGameObject* component);

#pragma region Network Events

	void AnEnemyPlayerWasShotByUs(UNetworkGameObject* characterWeHit, FString nameOfWeapon);

#pragma endregion

};