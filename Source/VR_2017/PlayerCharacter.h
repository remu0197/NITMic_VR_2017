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

	void UpFlashlight(float value);

	void RightFlashlight(float value);

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
		UStaticMeshComponent* m_Screen;

	UPROPERTY(EditAnywhere)
		float maxTraceDistance;

	UPROPERTY(EditAnywhere)
		UPointLightComponent* m_Flashlight;

	const float heightOfCellphone = 65.0f;
	const float distanceOfCellphone = -25.0f;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere)
		UCameraComponent* FirstPersonCamera;

	bool m_isOperateCellphone;
	
	unsigned int m_gotItemFlags;

	void PickupItem(enum class ItemName itemName);
	void LoseItem(enum class ItemName itemName);

	void SetIsOperateCellphone();
	void SetIsSquat();

	void Squat(float deltaTime);

	bool m_isSquat;

	const float correctDistance = 50.0f;

	float m_capsuleRadius;
	const float originalCapsuleRadius = 34.0f;

	int m_correctDirectionX : 2;
	int m_correctDirectionY : 2;

	UPROPERTY(EditAnywhere)
		float squatSpeed;

	UPROPERTY(EditAnywhere)
		float maxSquat;

	float lightUpAxis, lightRightAxis;

/******Debug*******/
private:
	static const float maxOpenAxis;

	static const float openSpeed;

	float m_openAxis,m_interval;

	UPROPERTY(EditAnywhere)
		float RastAmount;

	class ACellphoneManager* m_cellphone;
};
