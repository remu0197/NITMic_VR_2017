// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UsableActor.h"
#include <vector>
#include "DialBank.generated.h"

/**
 * 
 */
UCLASS()
class VR_2017_API ADialBank : public AUsableActor
{
	GENERATED_BODY()

public:
	ADialBank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual ItemName Event(const int innerProduct) override;

private:
	bool m_isOperateDial;
};
