// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "Engine.h"
#include "PlayerCharacter.h"
#include "UsableActor.h"
#include "DialBank2.h"
#include "PasscordManager.h"
#include "TimeManager.h"
#include <random>
#include <string>

//#ifndef VR_MODE_
//#define VR_MODE_
//#endif //VR_MODE_

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
	maxSquat(-25.0f),
	m_correctDirectionX(1),
	m_correctDirectionY(0),
	m_isOperateBank(false),
	m_multiInputValue(0.0f),
	m_stepTime(0.0f)
{
	m_capsuleRadius = originalCapsuleRadius;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = ObjectInitialier.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->TargetArmLength = 0.0f;
	CameraArm->RelativeLocation = FVector(0.0f, 0.0f, BaseEyeHeight);
	CameraArm->SetupAttachment(RootComponent);//AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//focas setting(need to 
	FirstPersonCamera->PostProcessSettings.DepthOfFieldMethod = EDepthOfFieldMethod::DOFM_BokehDOF;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldMethod = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalDistance = 50.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = false;
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

	FirstPersonCamera->SetupAttachment(CameraArm);

	//Allow the pawn to control rotation
	FirstPersonCamera->bUsePawnControlRotation = true;

	//m_cellphone = CreateDefaultSubobject<ACellphoneManager>(TEXT("Cellphone"));
	//m_cellphone->SetupAtta//AttachToComponent(FirstPersonCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	m_Flashlight->SetupAttachment(RootComponent);

	m_Flashlight->RelativeLocation = FVector(-50, 0, 0);

	m_UnderBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnderBodyMesh"));
	m_UnderBodyMesh->SetupAttachment(FirstPersonCamera);//AttachToComponent(FirstPersonCamera, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_TurnAxis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurnAxis"));
	m_TurnAxis->SetupAttachment(m_UnderBodyMesh);//AttachToComponent(m_UnderBodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_TopBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopBodyMesh"));
	m_TopBodyMesh->SetupAttachment(m_TurnAxis);//AttachToComponent(m_TurnAxis, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	m_Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	m_Screen->SetupAttachment(m_TurnAxis);//AttachToComponent(m_TurnAxis, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//set step height
	this->GetCharacterMovement()->MaxStepHeight = 10.0f;

	//set walkable angle
	//this->GetCharacterMovement()->SetWalkableFloorAngle = 45.0f;

	defaultCameraPos = FirstPersonCamera->GetRelativeTransform().GetLocation();

	parameters.Add("isCenter");
	parameters.Add("isRight");
	parameters.Add("isOne");
	parameters.Add("isTwo");
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_UnderBodyMesh->SetHiddenInGame(true);
	m_TurnAxis->SetHiddenInGame(true);
	m_TopBodyMesh->SetHiddenInGame(true);
	m_Screen->SetHiddenInGame(true);

	/*
	if (screenTextures.Num() != 0)
	{
		GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("dynamic"));

		screenMaterial = UMaterialInstanceDynamic::Create(m_Screen->GetMaterial(0), this);
		screenMaterial->SetTextureParameterValue("screen", screenTextures[0]);

		m_Screen->SetMaterial(0, screenMaterial);
	}
	*/
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeManager::AddPastTime(DeltaTime);

	if (m_isOperateCellphone)
	{
		if (m_openAxis > 20.0f)
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
					char str[] = "";
					ScreenInstance->SetScalarParameterValue(FName(str), 1.0f);
				}
			}
		}

		UCapsuleComponent *capsule = GetCapsuleComponent();

		//携帯がめり込まないために、当たり判定を一時補正
		if (m_capsuleRadius < 80.0f)
		{
			m_capsuleRadius += 0.5f;
			++m_correctDirectionX;
			++m_correctDirectionY;
			capsule->SetCapsuleRadius(m_capsuleRadius);
			this->GetCharacterMovement()->AddInputVector(FVector(0.f, correctDistance * m_correctDirectionY, 0.f));
			//= FVector(correctDistance * m_correctDirectionX, correctDistance * m_correctDirectionY, 0);
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

		AUsableActor* Usable = GetUsableInView();
		if (Usable)
		{
			if (Usable != currentFocusActor)
			{
				GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Focus in");
				currentFocusActor = Usable;
				currentFocusActor->StartFocus();
			}
		}
		else if(currentFocusActor)
		{
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Focus out");
			currentFocusActor->EndFocus();
			currentFocusActor = nullptr;
		}
	}

	Squat(DeltaTime);

	if (m_multiInputValue > 0.0f)
	{
		if (m_stepTime <= 0.0f)
		{
			int num = 0;
			int size = m_StepSounds.Num();
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step" + FString::FromInt(size));

			if (size > 1)
			{
				std::random_device seed;
				std::mt19937 engine(seed());
				std::uniform_int_distribution<> dist(0, m_StepSounds.Num() - 2);

				num = dist(engine);
				GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step" + FString::FromInt(num));

				USoundBase* temp = m_StepSounds[num];
				m_StepSounds[num] = m_StepSounds[size - 1];
				m_StepSounds[size - 1] = temp;
			}

			UGameplayStatics::PlaySoundAtLocation(this, m_StepSounds[size - 1], GetActorLocation());

			m_stepTime = 1.0f;
		}
		else
		{
			m_stepTime -= DeltaTime;
		}
	}
	else
	{
		m_stepTime = 0.0f;
	}

	m_multiInputValue = 0.0f;
}

const float APlayerCharacter::maxOpenAxis = 180.0f;
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

	InputComponent->BindAction("CancelEvent", IE_Pressed, this, &APlayerCharacter::SetIsOperateBank);

	InputComponent->BindAction("Squat", IE_Pressed, this, &APlayerCharacter::SetIsSquat);
}

void APlayerCharacter::MoveForward(float value)
{
	if ((Controller != NULL) && (value != 0) && !m_isOperateCellphone && !m_isOperateBank)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

		AddMovementInput(Direction, value);

		m_multiInputValue += value * value;
	}
}

void APlayerCharacter::MoveRight(float value)
{
	if(m_isOperateCellphone)
	{
		if (value < 0 && (cellphoneStep == 2 || cellphoneStep == 4))
		{
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			if (ScreenInstance != nullptr)
			{
				ScreenInstance->SetScalarParameterValue(FName(parameters[--cellphoneStep]), 0.0f);
			}
		}
		else if (value > 0 && (cellphoneStep == 1 || cellphoneStep == 3))
		{
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			if (ScreenInstance != nullptr)
			{
				ScreenInstance->SetScalarParameterValue(FName(parameters[cellphoneStep++]), 1.0f);
			}
		}
	}
	else if (m_isOperateBank)
	{
		if (m_currentOperateBank && m_currentOperateBank->OperateDial(value))
		{
			m_isOperateBank = false;
			unlockedBankList.Add(m_currentOperateBank);
		}
	}
	else
	{
		if ((Controller != NULL) && (value != 0))
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

			AddMovementInput(Direction, value);

			m_multiInputValue += value * value;
		}
	}
}

void APlayerCharacter::UpFlashlight(float value)
{
	if ((Controller != NULL) && (value != 0) && lightUpAxis * value <= 20.0f)
	{
		lightUpAxis += value;
		m_Flashlight->SetRelativeRotation(FQuat(FRotator(lightUpAxis, lightRightAxis, 0.0f)));

		m_multiInputValue += value * value;
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

void APlayerCharacter::OccurEvent()
{
	if (m_isOperateCellphone)
	{
		if (cellphoneStep == 0 || cellphoneStep == 2)
		{
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			if (ScreenInstance != nullptr)
			{
				ScreenInstance->SetScalarParameterValue(FName(parameters[cellphoneStep++]), 1.0f);
			}
		}
	}
	else
	{
		ItemName item;

		if (currentFocusActor)
		{
			//UsableActorのX軸ベクトルとUsableActorからPlayerへのベクトルの内積を計算したい
			FVector temp = currentFocusActor->GetTransform().GetUnitAxis(EAxis::X);
			FVector temp2 = currentFocusActor->GetActorLocation();
			temp2 = this->GetActorLocation() - temp2;
			float dir = FVector::DotProduct(temp, temp2);

			item = currentFocusActor->Event(dir);
			if (item == ItemName::bank)
			{
				ADialBank2* dial = dynamic_cast<ADialBank2*>(currentFocusActor);
				if (m_isOperateBank)
				{
					FirstPersonCamera->SetRelativeLocation(defaultCameraPos);
					m_isOperateBank = false;
				}
				else if (dial && unlockedBankList.Find(dial) == INDEX_NONE)
				{
					m_isOperateBank = true;
					m_currentOperateBank = dial;
					FVector pos = dial->GetCameraPos();
					float posZ = GetCapsuleComponent()->GetComponentLocation().Z;

					GetCapsuleComponent()->SetWorldLocation(FVector(pos.X, pos.Y, posZ));
					FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, pos.Z));
				}
			}
			else if(item != ItemName::noItem)
			{
				PickupItem(item);
			}

			currentFocusActor->StartFocus();
		}
		else
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
	int item = static_cast<int>(itemName);
	if ((m_gotItemFlags & (1 << item)) == 0) 
	{
		m_gotItemFlags |= (1 << static_cast<int>(itemName));
		if (m_shutterSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, m_shutterSound, GetActorLocation());
		}
	}
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

		//当たり判定の補正を解除
		m_capsuleRadius = originalCapsuleRadius;
		UCapsuleComponent* capsule = GetCapsuleComponent();
		capsule->SetCapsuleRadius(m_capsuleRadius);
	}
}

void APlayerCharacter::SetIsSquat()
{
	//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("close"));
	m_isSquat = !m_isSquat;
}

void APlayerCharacter::SetIsOperateBank()
{
	if (m_isOperateBank)
	{
		m_isOperateBank = false;
		FVector pos(0.0f, 0.0f, BaseEyeHeight);
		FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
	}
}

void APlayerCharacter::Squat(float deltaTime)
{
	float cameraHeight = CameraArm->GetRelativeTransform().GetLocation().Z;
	if (m_isSquat)
	{
		if (cameraHeight >= maxSquat)
		{
			cameraHeight -= squatSpeed * deltaTime;
			CameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, cameraHeight));
		}
	}
	else
	{
		if (cameraHeight <= BaseEyeHeight)
		{
			cameraHeight += squatSpeed * deltaTime;
			CameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, cameraHeight));
		}
	}
}