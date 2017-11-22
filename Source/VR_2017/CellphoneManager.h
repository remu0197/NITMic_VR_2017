// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellphoneManager.generated.h"

UCLASS()
class VR_2017_API ACellphoneManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACellphoneManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class StatusNode 
	{
	public:
		StatusNode(char* statusName);
		char* GetStatusName();

	private:
		char* _statusName;
	};

	class SceneNode
	{
	public:
		SceneNode(int row, int len);

		void MoveRight(int value);
		void MoveUp(int value);

		void Event();

		void Back();

		void AppendStatus(StatusNode* node);

		void SetRelatedScenes(SceneNode* nextScene, SceneNode* backScene);

	private:
		TArray<StatusNode*> statusList;

		const int MAX_RAW_COUNT, MAX_LEN_COUNT;

		int _currentRowCount, _currentLenCount;

		SceneNode* _nextScene;
		SceneNode* _backScene;
	};

	UPROPERTY(EditAnywhere)
		UMeshComponent* underBody;

	UPROPERTY(EditAnywhere)
		UMeshComponent* topBody;
	
};
