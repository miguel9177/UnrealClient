// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDPlayerHp.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UHUDPlayerHp : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "PlayerHUD")
        void UpdateTextHp(float PlayerHP);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"), Category = "Player HUD", DisplayName = "HPText")
    class UTextBlock* txtHp;
};
