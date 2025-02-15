// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "Engine.h"
#include "PlayerCharacter.h"
#include "UsableActor.h"


// Sets default values
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitialier) :
	m_isOperateCellphone(false),
	maxTraceDistance(250.0f),
	m_gotItemFlags(0),
	m_openAxis(160.0f),
	lightUpAxis(0.0f),
	lightRightAxis(0.0f),
	RastAmount(0.0f),
	m_interval(1.0f),
	squatSpeed(300.0f),
	m_isSquat(false),
	maxSquat(-25.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = ObjectInitialier.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));

	//focas setting(need to 
	FirstPersonCamera->PostProcessSettings.DepthOfFieldMethod = EDepthOfFieldMethod::DOFM_BokehDOF;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldMethod = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalDistance = 50.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalRegion = 300.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFocalRegion = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldNearTransitionRegion = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFarTransitionRegion = 1600.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldScale = 0.44f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldScale = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldNearBlurSize = 4.16f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFarBlurSize = 5.72;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = false;

	FirstPersonCamera->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//Position the camera a bit above the eyes
	FirstPersonCamera->RelativeLocation = FVector(0, 0, -1000);

	//Allow the pawn to control rotation
	FirstPersonCamera->bUsePawnControlRotation = true;

	m_Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	m_Flashlight->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_Flashlight->RelativeLocation = FVector(-50, 0, 0);

	m_UnderBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnderBodyMesh"));
	m_UnderBodyMesh->AttachToComponent(FirstPersonCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_TurnAxis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurnAxis"));
	m_TurnAxis->AttachToComponent(m_UnderBodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_TopBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopBodyMesh"));
	m_TopBodyMesh->AttachToComponent(m_TurnAxis, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	m_Screen->AttachToComponent(m_TurnAxis, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//set step height
	this->GetCharacterMovement()->MaxStepHeight = 10.0f;

	//set walkable angle
	//this->GetCharacterMovement()->SetWalkableFloorAngle = 45.0f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_UnderBodyMesh->SetHiddenInGame(true);
	m_TurnAxis->SetHiddenInGame(true);
	m_TopBodyMesh->SetHiddenInGame(true);
	m_Screen->SetHiddenInGame(true);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_isOperateCellphone)
	{
		if (m_openAxis > 0.0f)
		{
			m_openAxis -= openSpeed * DeltaTime;
			m_TurnAxis->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_openAxis)));
			m_UnderBodyMesh->SetRelativeLocation(FVector(heightOfCellphone * (1 - m_openAxis / maxOpenAxis), 0.0f, distanceOfCellphone));
		}
		else
		{
			m_interval -= DeltaTime;
			if (m_interval < 0.0f)
			{
				UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
				if (ScreenInstance != nullptr)
				{
					//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("close"));
					ScreenInstance->SetScalarParameterValue(FName("RastAmount"), 1.0f);
				}
			}
		}
	}
	else
	{
		if (m_openAxis < maxOpenAxis)
		{
			m_openAxis = maxOpenAxis;
			m_TurnAxis->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_openAxis)));
			m_UnderBodyMesh->SetRelativeLocation(FVector(heightOfCellphone * (1 - m_openAxis / maxOpenAxis), 0.0f, distanceOfCellphone));
		}
	}

	Squat(DeltaTime);
}

const float APlayerCharacter::maxOpenAxis = 160.0f;
const float APlayerCharacter::openSpeed = 180.0f;

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	InputComponent->BindAxis("TurnDebug", this, &APlayerCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUpDebug", this, &APlayerCharacter::AddControllerPitchInput);

	InputComponent->BindAxis("Turn", this, &APlayerCharacter::RightFlashlight/*AddControllerYawInput*/);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::UpFlashlight/*AddControllerPitchInput*/);

	InputComponent->BindAction("OccurEvent", IE_Pressed, this, &APlayerCharacter::OccurEvent);

	InputComponent->BindAction("OpenCellphone", IE_Pressed, this, &APlayerCharacter::SetIsOperateCellphone);

	InputComponent->BindAction("Squat", IE_Pressed, this, &APlayerCharacter::SetIsSquat);
}

void APlayerCharacter::MoveForward(float value)
{
	if ((Controller != NULL) && (value != 0) && !m_isOperateCellphone)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, value);
	}
}

void APlayerCharacter::MoveRight(float value)
{
	if ((Controller != NULL) && (value != 0) && !m_isOperateCellphone)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

		AddMovementInput(Direction, value);
	}
}

void APlayerCharacter::UpFlashlight(float value)
{
	if ((Controller != NULL) && (value != 0) && lightUpAxis * value <= 20.0f)
	{
		lightUpAxis += value;
		m_Flashlight->SetRelativeRotation(FQuat(FRotator(lightUpAxis, lightRightAxis, 0.0f)));
	}
}

void APlayerCharacter::RightFlashlight(float value)
{
	if ((Controller != NULL) && (value != 0) && lightRightAxis * value <= 30.0f)
	{
		lightRightAxis += value;
		m_Flashlight->SetRelativeRotation(FQuat(FRotator(lightUpAxis, lightRightAxis, 0.0f)));
	}
}

void APlayerCharacter::SquatView()
{

}

void APlayerCharacter::OccurEvent()
{
	if (!m_isOperateCellphone)
	{
		AUsableActor* Usable = GetUsableInView();
		ItemName item;

		//UsableActorのX軸ベクトルとUsableActorからPlayerへのベクトルの内積を計算したい
		FVector temp = Usable->GetTransform().GetUnitAxis(EAxis::X);
		FVector temp2 = Usable->GetActorLocation();
		temp2 = this->GetActorLocation() - temp2;
		float dir = FVector::DotProduct(temp, temp2);

		if (Usable)
		{
			item = Usable->Event(dir);
			if (item != ItemName::noItem)
			{
				PickupItem(item);
			}
		}
		else if (!Usable)
		{
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Can not Trace");
		}
	}
}

AUsableActor* APlayerCharacter::GetUsableInView()
{
	FVector CameraLoc;
	FRotator CameraRot;

	if (Controller == NULL)
		return NULL;

	Controller->GetPlayerViewPoint(CameraLoc, CameraRot);
	const FVector TraceStart = CameraLoc;
	const FVector Direction = CameraRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * maxTraceDistance);

	FCollisionQueryParams *TraceParams = new FCollisionQueryParams;

	FHitResult *Hit = new FHitResult;
	GetWorld()->LineTraceSingleByChannel(*Hit, TraceStart, TraceEnd, ECC_Visibility, *TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, true);

	return Cast<AUsableActor>(Hit->GetActor());
}

void APlayerCharacter::PickupItem(ItemName itemName)
{
	m_gotItemFlags |= (1 << static_cast<int>(itemName));
}

void APlayerCharacter::LoseItem(ItemName itemName)
{
	m_gotItemFlags &= ~(1 << static_cast<int>(itemName));
}

void APlayerCharacter::SetIsOperateCellphone()
{
	m_isOperateCellphone = !m_isOperateCellphone;
	m_UnderBodyMesh->SetHiddenInGame(!m_isOperateCellphone);
	m_TurnAxis->SetHiddenInGame(!m_isOperateCellphone);
	m_TopBodyMesh->SetHiddenInGame(!m_isOperateCellphone);
	m_Screen->SetHiddenInGame(!m_isOperateCellphone);

	//camera focas setting
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldMethod = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFocalRegion = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldScale = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = m_isOperateCellphone;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = m_isOperateCellphone;

	if (!m_isOperateCellphone)
	{
		m_interval = 1.0f;
		UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
		if (ScreenInstance != nullptr)
		{
			ScreenInstance->SetScalarParameterValue(FName("RastAmount"), 0.0f);
		}
	}
}

void APlayerCharacter::SetIsSquat()
{
	//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("close"));
	m_isSquat = !m_isSquat;
}

void APlayerCharacter::Squat(float deltaTime)
{
	float cameraHeight = FirstPersonCamera->GetRelativeTransform().GetLocation().Z;
	if (m_isSquat)
	{
		if (cameraHeight >= maxSquat)
		{
			cameraHeight -= squatSpeed * deltaTime;
			FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, cameraHeight));
		}
	}
	else
	{
		if (cameraHeight <= BaseEyeHeight)
		{
			cameraHeight += squatSpeed * deltaTime;
			FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, cameraHeight));
		}
	}
}