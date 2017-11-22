// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "CellphoneManager.h"


// Sets default values
ACellphoneManager::ACellphoneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACellphoneManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACellphoneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ACellphoneManager::StatusNode::StatusNode(char* statusName):
	_statusName(statusName)
{
	
}

char* ACellphoneManager::StatusNode::GetStatusName()
{
	return _statusName;
}

ACellphoneManager::SceneNode::SceneNode(int row, int len):
	MAX_RAW_COUNT(row),
	MAX_LEN_COUNT(len),
	_currentRowCount(0),
	_currentLenCount(0)
{

}

void ACellphoneManager::SceneNode::MoveRight(int value)
{
	if (value > 0 && _currentRowCount < MAX_RAW_COUNT - 1)
	{
		//�E�����ړ�
		++_currentRowCount;
	}
	else if (value < 0 && _currentRowCount > 0)
	{
		//�������ړ�
		--_currentRowCount;
	}
}

void ACellphoneManager::SceneNode::MoveUp(int value)
{
	if (value > 0 && _currentLenCount > 0)
	{
		//������ړ�
		--_currentLenCount;
	}
	else if (value < 0 && _currentLenCount < MAX_LEN_COUNT - 1)
	{
		//�������ړ�
		_currentLenCount++;
	}
}

void ACellphoneManager::SceneNode::Event()
{
	if (_nextScene != nullptr)
	{
		//���̏�Ԃ�
	}
}

void ACellphoneManager::SceneNode::Back()
{
	if (_backScene != nullptr)
	{
		//�O�̏�Ԃ�
	}
}

void ACellphoneManager::SceneNode::AppendStatus(StatusNode* node)
{
	//��ԃm�[�h�̒ǉ�
	statusList.Add(node);
}

void ACellphoneManager::SceneNode::SetRelatedScenes(SceneNode* nextScene, SceneNode* backScene)
{
	//�J�ڐ�̃V�[���m�[�h�̐ݒ�
	_nextScene = nextScene;
	_backScene = backScene;
}

