// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetworkGameObject.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNetworkGameObject : public UActorComponent
{
	GENERATED_BODY() //up to here code should be the same
	UPROPERTY(EditAnywhere)
	bool isLocallyOwned;
	UPROPERTY(VisibleAnywhere)
	int32 globalId;
	UPROPERTY(VisibleAnywhere)
	int32 localId;

	struct RequestedId 
	{
		bool requestedId = false;
		float timeIdWasRequested = 0;
	};
public:	
	static int32 lastLocalID;
	RequestedId requestedIdInfo;

	// Sets default values for this component's properties
	UNetworkGameObject();

	int32 GetGlobalID();
	int32 GetLocalID();
	bool GetIsLocallyOwned();
	void SetGlobalID(int32 gId);
	void SetLocalID(int32 lId);
	void FromPacket(FString packet);
	FString ToPacket();
	int32 GlobalIDFromPacket(FString packet);



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
