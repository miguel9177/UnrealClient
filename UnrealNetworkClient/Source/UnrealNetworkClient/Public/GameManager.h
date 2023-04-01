// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class UNetworkGameObject;
UCLASS()
class UNREALNETWORKCLIENT_API AGameManager : public AActor
{
	GENERATED_BODY()
	
private:
	//we make thius private since its a singleton	
	AGameManager();
	static AGameManager* instance;
	bool asSetudUpReferences;
	//this will store our player network gameobject, so that every scriptys can access our network gameobject
	UNetworkGameObject* ourPlayerNetworkGameObject;
	//this will store our player controller, so that every script can have access to this, this will allow other scripts to get other class that are childs of our actor
	APlayerController* ourPlayerController;
	bool bFoundNetworkGameObject = false;
public:	
	static AGameManager* GetInstance();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Getters

	UNetworkGameObject* GetOurPlayerNetworkGameObject();
	APlayerController* GetOurPlayerController();

#pragma endregion

};
