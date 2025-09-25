// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/ZackPlayerController.h"

AZackPlayerController::AZackPlayerController()
{
	ZackTeamID = FGenericTeamId(0);
}

FGenericTeamId AZackPlayerController::GetGenericTeamId() const
{
	return ZackTeamID;
}