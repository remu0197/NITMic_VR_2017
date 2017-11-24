// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "DoorActor.h"
#include "PlayerCharacter.h"


// Sets default values
ADoorActor::ADoorActor() :
	m_isOpen(false),
	doorAngle(0.0f),
	nobAngle(0.0f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_Parent = CreateDefaultSubobject<USceneComponent>(TEXT("Parent"));
	m_Parent->SetupAttachment(GetRootComponent());

	//m_BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	//m_BoxTrigger->bGenerateOverlapEvents = true;
	//m_BoxTrigger->>OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::TriggerEnter);
	//m_BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &ADoorActor::TriggerExit);
	//m_BoxTrigger->SetupAttachment(m_Parent);

	m_TurnAxis = CreateDefaultSubobject<UBoxComponent>(TEXT("TurnAxis"));
	m_TurnAxis->SetupAttachment(m_Parent);

	m_MyMesh->SetupAttachment(m_TurnAxis);

	m_KeyholeFront = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyholeFront"));
	m_KeyholeFront->SetupAttachment(m_MyMesh);

	m_NobFrontParent = CreateDefaultSubobject<USceneComponent>(TEXT("NobFrontParent"));
	m_NobFrontParent->SetupAttachment(m_KeyholeFront);

	m_DoorNobFront = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorNobFront"));
	m_DoorNobFront->SetupAttachment(m_NobFrontParent);

	m_KeyholeBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyholeBack"));
	m_KeyholeBack->SetupAttachment(m_MyMesh);

	m_NobBackParent = CreateDefaultSubobject<USceneComponent>(TEXT("NobBackParent"));
	m_NobBackParent->SetupAttachment(m_KeyholeBack);

	m_DoorNobBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorNobBack"));
	m_DoorNobBack->SetupAttachment(m_NobBackParent);
}

// Edit turn parameter here.
const float ADoorActor::openSpeed = 90.0f;
const float ADoorActor::maxOpenAngle = 90.0f;
const float ADoorActor::maxNobAngle = 180.0f;

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_isOpen)
	{
		OpenDoor(DeltaTime);
	}
}

ItemName ADoorActor::Event(const int innerProduct)
{
	if (innerProduct > 0)
		openDir = -1;
	else
		openDir = 1;

	if (m_SoundEffect != NULL && !m_isOpen)
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_SoundEffect, GetActorLocation());
	}

	m_isOpen = true;

	return ItemName::noItem;
}

void ADoorActor::TriggerExit(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	m_isOpen = false;
	//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Black, TEXT("close"));
}

void ADoorActor::OpenDoor(float deltaTime)
{
	if (nobAngle < maxNobAngle)
	{
		nobAngle += openSpeed * 2.0f * deltaTime;
		m_NobFrontParent->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, nobAngle)));
		m_NobBackParent->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, -nobAngle)));
	}
	else if (doorAngle * openDir < maxOpenAngle)
	{
		doorAngle += openSpeed * openDir * deltaTime;
		m_TurnAxis->SetRelativeRotation(FQuat(FRotator(0.0f, doorAngle, 0.0f)));
	}
	else
	{
		m_isOpen = false;
	}
}

void ADoorActor::CloseDoor(float deltaTime)
{
	if (doorAngle > 0.0f)
	{
		doorAngle -= openSpeed * deltaTime;
		m_TurnAxis->SetRelativeRotation(FQuat(FRotator(0.0f, doorAngle, 0.0f)));
	}
}
