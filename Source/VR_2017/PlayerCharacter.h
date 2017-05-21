// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class VR_2017_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float value);

	void MoveRight(float value);

	void OccurEvent();

	class AUsableActor* GetUsableInView();

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_TurnAxis;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_TopBodyMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_UnderBodyMesh;

	UPROPERTY(EditAnywhere)
		float maxTraceDistance;

	UCameraComponent* FirstPersonCamera;

	bool m_isOperateCellphone;
	
	unsigned int m_gotItemFlags;

	void PickupItem(enum class ItemName itemName);
	void LoseItem(enum class ItemName itemName);

	void SetIsOperateCellphone();

/******Debug*******/
private:
	static const float maxOpenAxis;
	static const float openSpeed;

	float m_openAxis;

};
