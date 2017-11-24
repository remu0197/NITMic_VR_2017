// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "Engine.h"
#include "PlayerCharacter.h"
#include "UsableActor.h"
#include "DialBank2.h"
#include "TimeManager.h"
#include "CellphoneManager.h"
#include "Matinee/MatineeActor.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
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
	m_stepTime(0.0f),
	dir(0.0f),
	_currentStatusName(""),
	pastTime(0.0f),
	cellphoneCorrectValue(25.0f),
	MinOpenAxis(25.0f),
	isEnd(false)
{
	m_capsuleRadius = originalCapsuleRadius;

	UCapsuleComponent *capsule = GetCapsuleComponent();
	capsule->SetCapsuleRadius(170.0f);

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
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalDistance = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFarTransitionRegion = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldScale = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldScale = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldNearBlurSize = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldFarBlurSize = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = false;
	FirstPersonCamera->PostProcessSettings.DepthOfFieldNearTransitionRegion = 0.0f;
	FirstPersonCamera->PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = false;

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

	/*
	SceneNode* wallpaper = new SceneNode("wallpaper");
	SceneNode* menu = new SceneNode("menu");
	SceneNode* pictures = new SceneNode("pictures");

	wallpaper->SetRelationNode(nullptr, menu);
	menu->SetRelationNode(wallpaper, pictures);
	pictures->SetRelationNode(menu, nullptr);

	cellphoneNodes.Add(new SceneNode(wallpaper));
	cellphoneNodes.Add(new SceneNode(menu));
	cellphoneNodes.Add(new SceneNode(pictures));

	_currentSceneNode = cellphoneNodes[0];

	delete wallpaper;
	delete menu;
	delete pictures;
	*/

	TSharedPtr<SceneNode> wallpaper(new SceneNode("wallpaper"));
	TSharedPtr<MenuNode> menu(new MenuNode(3, 3, 4));
	TSharedPtr<MenuNode> memo(new MenuNode(1, 3, 0));
	TSharedPtr<SceneNode> setting(new SceneNode("setting"));
	TSharedPtr<CameraNode> cameraMode(new CameraNode("camera"));
	TSharedPtr<LibraryNode> pictures(new LibraryNode(1, 5, 0));

	TSharedPtr<SceneNode> attention(new SceneNode("attention"));
	attention->SetRelationNode(menu, menu);

	TSharedPtr<SceneNode> menu1(new SceneNode("menu1"));
	menu1->SetRelationNode(menu, attention);
	menu->AppendNode(menu1);
	TSharedPtr<SceneNode> menu2(new SceneNode("menu2"));
	menu2->SetRelationNode(menu, attention);
	menu->AppendNode(menu2);
	TSharedPtr<SceneNode> menu3(new SceneNode("menu3"));
	menu3->SetRelationNode(menu, attention);
	menu->AppendNode(menu3);
	TSharedPtr<SceneNode> menu4(new SceneNode("menu4"));
	menu4->SetRelationNode(menu, attention);
	menu->AppendNode(menu4);
	TSharedPtr<SceneNode> menu5(new SceneNode("menu5"));
	menu5->SetRelationNode(menu, cameraMode);
	menu->AppendNode(menu5);
	TSharedPtr<SceneNode> menu6(new SceneNode("menu6"));
	menu6->SetRelationNode(menu, pictures);
	menu->AppendNode(menu6);
	TSharedPtr<SceneNode> menu7(new SceneNode("menu7"));
	menu7->SetRelationNode(menu, attention);
	menu->AppendNode(menu7);
	TSharedPtr<SceneNode> menu8(new SceneNode("menu8"));
	menu8->SetRelationNode(menu, memo);
	menu->AppendNode(menu8);
	TSharedPtr<SceneNode> menu9(new SceneNode("menu9"));
	menu9->SetRelationNode(menu, setting);
	menu->AppendNode(menu9);

	TSharedPtr<SceneNode> memo1(new SceneNode("memo1"));
	memo->AppendNode(memo1);
	TSharedPtr<SceneNode> memo2(new SceneNode("memo2"));
	memo->AppendNode(memo2);
	TSharedPtr<SceneNode> memo3(new SceneNode("memo3"));
	memo->AppendNode(memo3);

	TSharedPtr<SceneNode> picture1(new SceneNode("book"));
	pictures->AppendNode(picture1);
	TSharedPtr<SceneNode> picture2(new SceneNode("paper"));
	pictures->AppendNode(picture2);
	TSharedPtr<SceneNode> picture3(new SceneNode("key"));
	pictures->AppendNode(picture3);
	TSharedPtr<SceneNode> picture4(new SceneNode("clip"));
	pictures->AppendNode(picture4);
	TSharedPtr<SceneNode> picture5(new SceneNode("item"));
	pictures->AppendNode(picture5);

	wallpaper->SetRelationNode(nullptr, menu);
	menu->SetRelationNode(wallpaper, nullptr);
	memo->SetRelationNode(menu, nullptr);
	setting->SetRelationNode(menu, nullptr);
	pictures->SetRelationNode(menu, nullptr);
	cameraMode->SetRelationNode(menu, menu);
	cameraMode->SetLibrary(pictures);

	cellphoneNodes.Add(wallpaper);
	cellphoneNodes.Add(menu);
	cellphoneNodes.Add(pictures);
	cellphoneNodes.Add(cameraMode);
	cellphoneNodes.Add(memo);
	cellphoneNodes.Add(setting);

	_currentSceneNode = setting;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//c

	m_UnderBodyMesh->SetHiddenInGame(true);
	m_TurnAxis->SetHiddenInGame(true);
	m_TopBodyMesh->SetHiddenInGame(true);
	m_Screen->SetHiddenInGame(true);

	int count = 0;

	TSharedPtr<SceneNode> wallpaper(new SceneNode("wallpaper"));
	cellphoneNodes.Add(wallpaper);
	for (int i = 0; i < cellphoneNodes.Num(); ++i)
	{
		if (cellphoneNodes[i].IsValid())
		{
			count++;
		}
	}

	/*
	if (screenTextures.Num() != 0)
	{
		screenMaterial = UMaterialInstanceDynamic::Create(m_Screen->GetMaterial(0), this);
		screenMaterial->SetTextureParameterValue("screen", screenTextures[0]);

		m_Screen->SetMaterial(0, screenMaterial);
	}
	*/

	GetWorldTimerManager().SetTimer(handle, this, &APlayerCharacter::FinishGame, 9.9f, true, 600.0f);

	UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
	ScreenInstance->SetScalarParameterValue(FName("setting"), 1.0f);
	_currentStatusName = "setting";
	SetIsOperateCellphone();
}

void APlayerCharacter::FinishGame()
{
	if (!isEnd)
	{
		isEnd = true;
		class APlayerController * MyPC = Cast<APlayerController>(Controller);
		MyPC->ClientSetCameraFade(true, FColor::Black, FVector2D(0.0, 1.0), 10.0);
		//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step");
	}
	else
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), 0, EQuitPreference::Quit);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_isOperateCellphone)
	{
		if (m_openAxis > MinOpenAxis)
		{
			m_openAxis -= openSpeed * DeltaTime;
			m_TurnAxis->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_openAxis)));
			m_UnderBodyMesh->SetRelativeLocation(FVector(heightOfCellphone * (1 - m_openAxis / maxOpenAxis) - cellphoneCorrectValue, 0.0f, distanceOfCellphone));
			
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

		SetDofField(0.001f);
	}
	else
	{
		if (m_openAxis < maxOpenAxis)
		{
			m_openAxis += openSpeed * DeltaTime;
			m_TurnAxis->SetRelativeRotation(FQuat(FRotator(0.0f, 0.0f, m_openAxis)));
			m_UnderBodyMesh->SetRelativeLocation(FVector(heightOfCellphone * (1 - m_openAxis / maxOpenAxis) - -cellphoneCorrectValue - cellphoneCorrectValue, 0.0f, distanceOfCellphone));
			if (m_openAxis >= maxOpenAxis)
			{
				SetHiddenCellphone(true);
			}
			m_UnderBodyMesh->SetRelativeLocation(FVector(heightOfCellphone * (1 - m_openAxis / maxOpenAxis), 0.0f, distanceOfCellphone));
		}

		AUsableActor* Usable = GetUsableInView();
		if (Usable)
		{
			if (Usable != currentFocusActor)
			{
				currentFocusActor = Usable;
				currentFocusActor->StartFocus();
			}
		}
		else if(currentFocusActor)
		{
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
			//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step" + FString::FromInt(size));

			if (size > 1)
			{
				std::random_device seed;
				std::mt19937 engine(seed());
				std::uniform_int_distribution<> dist(0, m_StepSounds.Num() - 2);

				num = dist(engine);
				//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step" + FString::FromInt(num));

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
	InputComponent->BindAction("OccurEvent", IE_Released, this, &APlayerCharacter::FinishOperateBank);

	InputComponent->BindAction("OpenCellphone", IE_Pressed, this, &APlayerCharacter::SetIsOperateCellphone);

	InputComponent->BindAction("CancelEvent", IE_Pressed, this, &APlayerCharacter::CancelEvent);
	//InputComponent->BindAction("CancelEvent", IE_Pressed, this, &APlayerCharacter::SetIsOperateBank);

	InputComponent->BindAction("Squat", IE_Pressed, this, &APlayerCharacter::SetIsSquat);
}

void APlayerCharacter::MoveForward(float value)
{
	if (m_isOperateCellphone)
	{
		FName statusName = _currentSceneNode->MoveUp(value);
		if (statusName.Compare("") != 0)
		{
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			if (ScreenInstance != nullptr)
			{
				ScreenInstance->SetScalarParameterValue(FName(_currentStatusName), 0.0f);
				_currentStatusName = statusName;
				ScreenInstance->SetScalarParameterValue(FName(statusName), 1.0f);
				UGameplayStatics::PlaySoundAtLocation(this, m_operateSound, GetActorLocation());
			}
		}

	}
	else if ((Controller != NULL) && (value != 0) && !m_isOperateBank)
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
		FName statusName = _currentSceneNode->MoveRight(value);
		if (statusName.Compare("") != 0)
		{
			//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, statusName);
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			if (ScreenInstance != nullptr)
			{
				ScreenInstance->SetScalarParameterValue(FName(_currentStatusName), 0.0f);
				_currentStatusName = statusName;
				ScreenInstance->SetScalarParameterValue(FName(statusName), 1.0f);
				UGameplayStatics::PlaySoundAtLocation(this, m_operateSound, GetActorLocation());
			}
		}
	}
	else if (m_isOperateBank)
	{
		if (m_currentOperateBank && m_currentOperateBank->OperateDial(value))
		{
			m_isOperateBank = false;
			unlockedBankList.Add(m_currentOperateBank);
			FinishGame();
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
	static int i = 0;
	if (isEnd)
	{
		return;
	}
	if (m_isOperateCellphone)
	{
		TSharedPtr<SceneNode> ptr = _currentSceneNode->GetNextNode();

		if (ptr.IsValid())
		{
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			FName status = ptr->GetStatusName();

			if (ScreenInstance != nullptr && status.Compare("") != 0)
			{
				if (status.Compare("camera") == 0)
				{
					
					if (currentFocusActor != nullptr && currentFocusActor->GetItemName() != ItemName::door && currentFocusActor->GetItemName() != ItemName::bank && currentFocusActor->GetItemName() != ItemName::noItem && ptr->RegisterToLibrary(currentFocusActor))
					{
						ScreenInstance->SetScalarParameterValue(_currentStatusName, 0.0f);

						_currentStatusName = currentFocusActor->GetItemFName();
						_currentSceneNode = ptr;

						ScreenInstance->SetScalarParameterValue(_currentStatusName, 1.0f);

						currentFocusActor = nullptr;

						if (m_shutterSound != NULL)
						{
							UGameplayStatics::PlaySoundAtLocation(this, m_shutterSound, GetActorLocation());
						}
					}
					
				}
				else
				{
					
					ScreenInstance->SetScalarParameterValue(_currentStatusName, 0.0f);

					_currentStatusName = status;
					_currentSceneNode = ptr;

					ScreenInstance->SetScalarParameterValue(_currentStatusName, 1.0f);

					if (m_decideSound != NULL)
					{
						UGameplayStatics::PlaySoundAtLocation(this, m_decideSound, GetActorLocation());
					}
				}
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
			dir = FVector::DotProduct(temp, temp2);

			item = currentFocusActor->Event(dir);
			if (item == ItemName::bank)
			{
				ADialBank2* dial = dynamic_cast<ADialBank2*>(currentFocusActor);
				if (dial && unlockedBankList.Find(dial) == INDEX_NONE)
				{
					m_isOperateBank = true;
					m_currentOperateBank = dial;
					m_isSquat = false;
					//CameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight));
					//FVector pos = dial->GetCameraPos();
					//float posZ = GetCapsuleComponent()->GetComponentLocation().Z;

					//GetCapsuleComponent()->SetWorldLocation(FVector(pos.X, pos.Y, posZ));
					//FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, pos.Z));
				}
			}

			currentFocusActor->StartFocus();
		}
	}
}

void APlayerCharacter::FinishOperateBank()
{
	if (m_isOperateBank)
	{
		//FirstPersonCamera->SetRelativeLocation(defaultCameraPos);
		//FVector pos = GetCapsuleComponent()->GetComponentLocation();
		//GetCapsuleComponent()->SetWorldLocation(FVector(pos.X + (originalCapsuleRadius + 20.0f)*(dir / dir), pos.Y, pos.Z));

		m_isOperateBank = false;
	}
}

void APlayerCharacter::CancelEvent()
{
	if (m_isOperateCellphone)
	{
		TSharedPtr<SceneNode> ptr = _currentSceneNode->GetBackNode();
		if (ptr.IsValid())
		{
			UMaterialInstanceDynamic* ScreenInstance = m_Screen->CreateDynamicMaterialInstance(0);
			FName status = ptr->GetStatusName();
			if (ScreenInstance != nullptr && status.Compare("") != 0)
			{
				ScreenInstance->SetScalarParameterValue(_currentStatusName, 0.0f);
				_currentStatusName = status;
				ScreenInstance->SetScalarParameterValue(_currentStatusName, 1.0f);
			}
			_currentSceneNode = ptr;
			UGameplayStatics::PlaySoundAtLocation(this, m_cancelSound, GetActorLocation());
		}
	}
	else if (m_isOperateBank)
	{
		FirstPersonCamera->SetRelativeLocation(defaultCameraPos);
		FVector pos = GetCapsuleComponent()->GetComponentLocation();
		GetCapsuleComponent()->SetWorldLocation(FVector(pos.X + (originalCapsuleRadius + 20.0f)*(dir / dir), pos.Y, pos.Z));

		m_isOperateBank = false;
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
	if (isEnd)
	{
		return;
	}

	m_isOperateCellphone = !m_isOperateCellphone;

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

		//当たり判定の補正を解除
		m_capsuleRadius = originalCapsuleRadius;
		UCapsuleComponent* capsule = GetCapsuleComponent();
		capsule->SetCapsuleRadius(m_capsuleRadius);

		SetDofField(0.0f);
	}
	else 
	{
		SetHiddenCellphone(false);
		if (currentFocusActor != nullptr)
		{
			currentFocusActor->EndFocus();
		}
	}
}

void APlayerCharacter::SetHiddenCellphone(bool flag)
{
	m_UnderBodyMesh->SetHiddenInGame(flag);
	m_TurnAxis->SetHiddenInGame(flag);
	m_TopBodyMesh->SetHiddenInGame(flag);
	m_Screen->SetHiddenInGame(flag);
}

void APlayerCharacter::SetIsSquat()
{
	if(!m_isOperateBank)
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

void APlayerCharacter::SetDofField(float value)
{
	if (value <= 0.0f)
	{
		FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalDistance = 0.0f;
		FirstPersonCamera->PostProcessSettings.DepthOfFieldFarTransitionRegion = 0.0f;
		FirstPersonCamera->PostProcessSettings.DepthOfFieldScale = 0.0f;
		FirstPersonCamera->PostProcessSettings.DepthOfFieldNearBlurSize = 0.0f;
		FirstPersonCamera->PostProcessSettings.DepthOfFieldFarBlurSize = 0.0f;
	}
	else
	{
		if(FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalDistance <= DoF_FocalDistance)
			FirstPersonCamera->PostProcessSettings.DepthOfFieldFocalDistance = DoF_FocalDistance - cellphoneCorrectValue;
		if(FirstPersonCamera->PostProcessSettings.DepthOfFieldFarTransitionRegion < DoF_FarTransitionRegion)
			FirstPersonCamera->PostProcessSettings.DepthOfFieldFarTransitionRegion += value * 10000.0f;
		if(FirstPersonCamera->PostProcessSettings.DepthOfFieldScale < DoF_FieldScale)
			FirstPersonCamera->PostProcessSettings.DepthOfFieldScale += value;
		if(FirstPersonCamera->PostProcessSettings.DepthOfFieldNearBlurSize < DoF_NearBlurSize)
			FirstPersonCamera->PostProcessSettings.DepthOfFieldNearBlurSize += value;
		if(FirstPersonCamera->PostProcessSettings.DepthOfFieldFarBlurSize < DoF_FarBlurSize)
			FirstPersonCamera->PostProcessSettings.DepthOfFieldFarBlurSize += value;
	}
}

/*****************************************************************************************************************************/

APlayerCharacter::SceneNode::SceneNode(FName statusName):
	_statusName(statusName)
{

}

void APlayerCharacter::SceneNode::SetRelationNode(TSharedPtr<SceneNode> back, TSharedPtr<SceneNode> next)
{
	if(back.IsValid())
		_backNode = back;
	if(next.IsValid())
		_nextNode = next;
}

TSharedPtr<APlayerCharacter::SceneNode> APlayerCharacter::SceneNode::GetBackNode()
{
	return _backNode;
}

TSharedPtr<APlayerCharacter::SceneNode> APlayerCharacter::SceneNode::GetNextNode()
{
	return _nextNode;
}

FName APlayerCharacter::SceneNode::GetStatusName()
{
	return _statusName;
}

/******************************************************************************************************************************/

APlayerCharacter::MenuNode::MenuNode(int len, int row, int defaultPos) :
	SceneNode(""),
	MAX_LEN_COUNT(len),
	MAX_ROW_COUNT(row),
	DEFAULT_POS(defaultPos),
	_currentLenCount(0),
	_currentRowCount(0),
	_lenIntervalCount(0),
	_rowIntervalCount(0)
{
	_currentLenCount = defaultPos / MAX_ROW_COUNT;
	_currentRowCount = defaultPos % MAX_LEN_COUNT;
}

TSharedPtr<APlayerCharacter::SceneNode> APlayerCharacter::MenuNode::GetNextNode()
{
	int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;

	if (contentNodes.Num() > index)
	{
		return contentNodes[index]->GetNextNode();
	}

	return nullptr;
}

void APlayerCharacter::MenuNode::AppendNode(TSharedPtr<SceneNode> node)
{
	if (node.IsValid())
	{
		contentNodes.Add(node);
	}
}

FName APlayerCharacter::MenuNode::GetStatusName()
{
	_currentLenCount = DEFAULT_POS / MAX_ROW_COUNT;
	_currentRowCount = DEFAULT_POS % MAX_LEN_COUNT;

	int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;

	return contentNodes[index]->GetStatusName();
}

FName APlayerCharacter::MenuNode::MoveUp(float value)
{
	if (value != 0.0f && --_lenIntervalCount > 0)
	{
		return "";
	}
	else if (value == 0.0f)
	{
		_lenIntervalCount = 0;
	}

	if (value < 0.0f && _currentLenCount < MAX_LEN_COUNT - 1)
	{
		++_currentLenCount;
		int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
		_lenIntervalCount = 30;
		return contentNodes[index]->GetStatusName();
	}
	else if (value > 0.0f && _currentLenCount > 0)
	{
		--_currentLenCount;
		int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
		_lenIntervalCount = 30;
		return contentNodes[index]->GetStatusName();
	}

	return "";
}

FName APlayerCharacter::MenuNode::MoveRight(float value)
{
	if (value != 0.0f && --_rowIntervalCount > 0)
	{
		return "";
	}
	else if (value == 0.0f)
	{
		_rowIntervalCount = 0;
	}

	if (value > 0.0f && _currentRowCount < MAX_ROW_COUNT - 1)
	{
		++_currentRowCount;
		int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
		_rowIntervalCount = 30;
		return contentNodes[index]->GetStatusName();

	}
	else if (value < 0.0f && _currentRowCount > 0)
	{
		--_currentRowCount;
		int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
		_rowIntervalCount = 30;
		return contentNodes[index]->GetStatusName();
	}

	return "";
}

/***********************************************************************************************************************************/

APlayerCharacter::LibraryNode::LibraryNode(int len, int row, int defaultPos) :
	MenuNode(len, row, defaultPos),
	_flags(0)
{

}

bool APlayerCharacter::LibraryNode::SetFlag(int flag)
{
	if (((1 << flag) & _flags) == 0)
	{
		_flags |= (1 << flag);
		return true;
	}

	return false;
}

bool APlayerCharacter::LibraryNode::FindFlag(int flag)
{
	//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, "Step" + FString::FromInt(flag));
	if (flag < 5 && ((1 << flag) & _flags) != 0)
	{
		return true;
	}

	return false;
}

FName APlayerCharacter::LibraryNode::GetStatusName()
{
	for (int i = 0; i < contentNodes.Num(); ++i)
	{
		if (FindFlag(i))
		{
			GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, FString::FromInt(_currentRowCount));
			_currentRowCount = i;
			return contentNodes[i]->GetStatusName();
		}
	}

	return "";
}

FName APlayerCharacter::LibraryNode::MoveUp(float value)
{
	int tempCount = _currentLenCount;

	if (value != 0.0f && --_lenIntervalCount > 0)
	{
		return "";
	}
	else if (value == 0.0f)
	{
		_lenIntervalCount = 0;
	}

	if (value < 0.0f && _currentLenCount < MAX_LEN_COUNT - 1)
	{
		for (++_currentLenCount; _currentLenCount < MAX_LEN_COUNT; ++_currentLenCount)
		{
			if (FindFlag(_currentLenCount))
			{
				int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
				_lenIntervalCount = 30;
				return contentNodes[index]->GetStatusName();
			}
		}
	}
	else if (value > 0.0f && _currentLenCount > 0)
	{
		for (--_currentLenCount; _currentLenCount >= 0; --_currentLenCount)
		{
			if (FindFlag(_currentLenCount))
			{
				int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
				_lenIntervalCount = 30;
				return contentNodes[index]->GetStatusName();
			}
		}
	}

	_currentLenCount = tempCount;

	return "";
}

FName APlayerCharacter::LibraryNode::MoveRight(float value)
{
	int tempCount = _currentRowCount;

	if (value != 0.0f && --_rowIntervalCount > 0)
	{
		return "";
	}
	else if (value == 0.0f)
	{
		_rowIntervalCount = 0;
	}

	if (value < 0.0f && _currentRowCount < MAX_ROW_COUNT - 1)
	{
		for (++_currentRowCount; _currentRowCount < MAX_ROW_COUNT; ++_currentRowCount)
		{
			if (FindFlag(_currentRowCount))
			{
				int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
				_rowIntervalCount = 30;
				GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, FString::FromInt(_currentRowCount));
				return contentNodes[index]->GetStatusName();
			}
		}
	}
	else if (value > 0.0f && _currentRowCount > 0)
	{
		for (--_currentRowCount; _currentRowCount >= 0; --_currentRowCount)
		{
			if (FindFlag(_currentRowCount))
			{
				int index = _currentRowCount + _currentLenCount * MAX_ROW_COUNT;
				_rowIntervalCount = 30;
				GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, FString::FromInt(_currentRowCount));
				return contentNodes[index]->GetStatusName();
			}
		}
	}

	_currentRowCount = tempCount;

	return "";
}

/**********************************************************************************************************************/

APlayerCharacter::CameraNode::CameraNode(FName statusName) :
	SceneNode(statusName)
{

}

void APlayerCharacter::CameraNode::SetLibrary(TSharedPtr<LibraryNode> library)
{
	if (library.IsValid())
	{
		targetLibrary = library;
	}
}

bool APlayerCharacter::CameraNode::RegisterToLibrary(AUsableActor* focusActor)
{
	if (focusActor != nullptr)
	{
		int targetItem = static_cast<int>(focusActor->GetItemName());
		if (!targetLibrary->FindFlag(targetItem) && targetLibrary->SetFlag(targetItem))
		{
			return true;
		}
	}

	return false;
}