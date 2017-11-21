// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "DialBank2.h"

ADialBank2::ADialBank2():
	m_value(0.0f),
	m_isTurningDial(false),
	m_currentDialAxis(0.0f),
	m_currentTurnDir(DialTurnDir::Left),
	PasscordFirstDir(DialTurnDir::Left),
	m_currentNextDir(DialTurnDir::Left),
	m_currentDialNo(0),
	m_currentTurnCount(0),
	m_dialScaleValue(0.0f),
	m_currentPassPhaseCount(0),
	cameraPlusPos(100.0f, 0.0f, -100.0f),
	cameraPos(),
	m_isOpened(false),
	m_currentHandleAxis(0.0f),
	m_currentDoorAxis(0.0f)
{
	DoorParent = CreateDefaultSubobject<USceneComponent>(TEXT("DoorParent"));
	DoorParent->SetupAttachment(m_MyMesh);

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(DoorParent);

	m_HandleParent = CreateDefaultSubobject<USceneComponent>(TEXT("HandleParent"));
	m_HandleParent->SetupAttachment(Door);

	Handle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle"));
	Handle->SetupAttachment(m_HandleParent);

	Dial = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dial"));
	Dial->SetupAttachment(Door);

	int32 NoArray[] = { 10, 5, 8 };
	PasscordNo.Append(NoArray, ARRAY_COUNT(NoArray));

	int32 TurnCountArray[] = { 4, 3, 2 };
	PasscordTurnCount.Append(TurnCountArray, ARRAY_COUNT(TurnCountArray));
}

void ADialBank2::BeginPlay()
{
	Super::BeginPlay();

	m_dialScaleValue = 360.0f / DialTotalScaleCount;

	m_currentNextDir = PasscordFirstDir;

	FVector loc = Dial->GetComponentLocation();
	cameraPos = FVector(loc.X, loc.Y, loc.Z) + cameraPlusPos;
}


void ADialBank2::Tick(float DeltaTime)
{
	if (m_isOpened)
	{
		if (m_currentHandleAxis >= -60.0f)
		{
			m_currentHandleAxis -= 1.0f;
			m_HandleParent->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_currentHandleAxis)));
		}
		else if (m_currentDoorAxis >= -120.0f)
		{
			m_currentDoorAxis -= 1.0f;
			DoorParent->SetRelativeRotation(FQuat(FRotator(0.0f, m_currentDoorAxis, 0.0f)));
		}
	}

	if (m_isTurningDial)
	{
		if (m_currentTurnDir == DialTurnDir::Right)
		{
			m_currentDialAxis += m_dialScaleValue / 30.0f;
			if (m_currentDialAxis >= m_dialScaleValue * (m_currentDialNo + 1))
			{
				ChangeDialNo(1, DialTurnDir::Left);
			}
		}
		else if (m_currentTurnDir == DialTurnDir::Left)
		{
			m_currentDialAxis -= m_dialScaleValue / 30.0f;
			if (m_currentDialAxis <= m_dialScaleValue * (m_currentDialNo - 1))
			{
				ChangeDialNo(-1, DialTurnDir::Right);
			}
		}

		Dial->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_currentDialAxis)));
	}
}

ItemName ADialBank2::Event(const int innerProduct)
{
	return ItemName::bank;
}

bool ADialBank2::OperateDial(float value)
{
	if (!m_isTurningDial)
	{
		if (value > 0.0f)
		{
			m_isTurningDial = true;
			m_currentTurnDir = DialTurnDir::Right;
			if (m_SoundEffect != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, m_SoundEffect, GetActorLocation());
			}
		}
		else if (value < 0.0f)
		{
			m_isTurningDial = true;
			m_currentTurnDir = DialTurnDir::Left;
			if (m_SoundEffect != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, m_SoundEffect, GetActorLocation());
			}
		}
	}

	return m_isOpened;
}

FVector ADialBank2::GetCameraPos()
{
	return cameraPos;
}

void ADialBank2::ChangeDialNo(int additionValue, DialTurnDir changeDirValue)
{
	if (m_currentTurnDir != m_currentNextDir)
	{
		m_currentNextDir = PasscordFirstDir;
		m_currentPassPhaseCount = 0;
		GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Fail");
	}

	m_currentDialNo += additionValue;
	if (m_currentDialNo < 0)
	{
		m_currentDialNo = DialTotalScaleCount - 1;
	}
	else if (m_currentDialNo >= DialTotalScaleCount)
	{
		m_currentDialNo = 0;
	}

	if (m_currentDialNo == PasscordNo[m_currentPassPhaseCount])
	{
		++m_currentTurnCount;
		if (m_currentTurnCount == PasscordTurnCount[m_currentPassPhaseCount])
		{
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step");
			m_currentTurnCount = 0;
			if (++m_currentPassPhaseCount == PasscordNo.Num())
			{
				GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Success");
				m_currentPassPhaseCount = 0;
				m_isOpened = true;
			}
			m_currentNextDir = changeDirValue;
		}
	}

	m_currentDialAxis = m_dialScaleValue * m_currentDialNo;
	m_isTurningDial = false;
}

