// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponRaycast.generated.h"

class UCharacterHealth;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UWeaponRaycast : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float amountOfDamageToGivePerShot = 10;

	APlayerController* myPlayerController;
public:	
	// Sets default values for this component's properties
	UWeaponRaycast();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetupInputs(UInputComponent* playerInput);
	void FireButtonClicked();
	UCharacterHealth* FireRaycast();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
