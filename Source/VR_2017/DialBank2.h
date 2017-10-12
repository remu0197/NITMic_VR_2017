// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UsableActor.h"
#include "DialBank2.generated.h"

/*
*
*/

UENUM()
enum class DialTurnDir
{
	Left,
	Right
};

UCLASS()
class VR_2017_API ADialBank2 : public AUsableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADialBank2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual ItemName Event(const int innerProduct) override;

	void OperateDial(float value);
	
	float m_value;

	FVector GetCameraPos();

private:
	bool m_isTurningDial;

	int m_currentDialNo;
	int m_currentTurnCount;

	float m_dialScaleValue;
	float m_currentDialAxis;

	int m_currentPassPhaseCount;

	DialTurnDir m_currentTurnDir;

	const int DialTotalScaleCount = 12;

	DialTurnDir m_currentNextDir;

	UPROPERTY(EditAnywhere)
		DialTurnDir PasscordFirstDir;

	UPROPERTY(EditAnywhere)
		TArray<int> PasscordTurnCount;

	UPROPERTY(EditAnywhere)
		TArray<int> PasscordNo;

	UPROPERTY(EditAnywhere)
		USceneComponent* DialComponent;

	UPROPERTY(EditAnywhere)
		FVector cameraPos;

	void ChangeDialNo(int additionValue, DialTurnDir changeDirValue);


};
