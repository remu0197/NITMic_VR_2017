// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UsableActor.generated.h"

UENUM()
enum class ItemName
{
	book,
	paper,
	key,
	clip,
	noItem,
	bank,
	door,
};

UCLASS()
class VR_2017_API AUsableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUsableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual ItemName Event(const int innerProduct);

	ItemName GetItemName()
	{
		return m_itemName;
	}

	FName GetItemFName();

protected:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_MyMesh;

	UPROPERTY(EditAnywhere)
		USoundBase* m_SoundEffect;

	TArray<UMeshComponent*> Meshs;

public:
	void StartFocus();
	void EndFocus();

private:
	UPROPERTY(EditAnywhere)
		ItemName m_itemName;
};
