// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "DialBank.h"
#include "PasscordManager.h"

ADialBank::ADialBank():
	m_isOperateDial(true),
	m_currentDialNo(0.0f),
	m_dialScaleValue(0.0f),
	m_currentTurnCount(0.0f)
{
	Dial = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dial"));
}

void ADialBank::BeginPlay()
{
	m_dialScaleValue = 360.0f / DialTotalScaleCount;

}

void ADialBank::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);


	////GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("kajspfei"));
	//m_MyMesh->SetRelativeRotation(FQuat(FRotator(100.0f, 0.0f, 0.0f)));
	//

	//if (m_isOperateDial)
	//{
	//	float axisX = Dial->RelativeRotation.Roll;
	//	if (axisX < 0.0f)
	//	{
	//		Dial->RelativeRotation.Roll = axisX + 360.0f;
	//	}
	//	else if (axisX > 360.0f)
	//	{
	//		Dial->RelativeRotation.Roll = axisX - 360.0f;
	//	}
	//}

	Super::Tick(DeltaTime);

	m_currentDialNo += 1.0f;

	m_MyMesh->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_currentDialNo)));

	GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("kajspfei"));
}

ItemName ADialBank::Event(const int innerProduct)
{
	return ItemName::bank;
}


