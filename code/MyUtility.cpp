// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUtility.h"

void SwapVectors(FVector & Vec1, FVector & Vec2)
{
	FVector temp = Vec1;

	Vec1 = Vec2;
	Vec2 = temp;
}

void BooleanToggle(bool & boolVar)
{
	boolVar = !boolVar;
}
