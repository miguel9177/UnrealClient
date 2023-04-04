// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHealth.h"

// Sets default values for this component's properties
UCharacterHealth::UCharacterHealth()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	amountOfHealth = 100;
	// ...
}


// Called when the game starts
void UCharacterHealth::BeginPlay()
{
	Super::BeginPlay();

	if (hpTextRenderer)
	{
		hpTextRenderer->SetText(FText::FromString("Your new text here"));
	}
	
}


// Called every frame
void UCharacterHealth::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCharacterHealth::TakeDamage(float _amountOfDamageToTake)
{
	amountOfHealth -= _amountOfDamageToTake;
}

float UCharacterHealth::GetAmountOfHealth()
{
	return amountOfHealth;
}

