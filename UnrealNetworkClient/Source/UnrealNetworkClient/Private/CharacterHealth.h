// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUDPlayerHp.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextRenderComponent.h"
#include "CharacterHealth.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UCharacterHealth : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterAmountOfHealth")
	float amountOfHealth = 100;


#pragma region UI

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIHpText")
		UTextRenderComponent* hpTextRenderer;

#pragma endregion


	// Sets default values for this component's properties
	UCharacterHealth();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TakeDamage(float _amountOfDamageToTake);

	UFUNCTION(BlueprintCallable, Category = "GetAmountOfHealth")
	float GetAmountOfHealth();
};
