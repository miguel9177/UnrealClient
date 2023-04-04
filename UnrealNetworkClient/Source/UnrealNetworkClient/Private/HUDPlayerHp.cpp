// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDPlayerHp.h"
#include "Components/TextBlock.h"

void UHUDPlayerHp::UpdateTextHp(float PlayerHP)
{
	if (txtHp)
	{
		txtHp->SetText(FText::AsNumber(PlayerHP));
	}
}
