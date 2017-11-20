// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellphoneManager.generated.h"

UCLASS()
class VR_2017_API ACellphoneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACellphoneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		UMeshComponent* underBody;

	UPROPERTY(EditAnywhere)
		UMeshComponent* topBody;
	
};
