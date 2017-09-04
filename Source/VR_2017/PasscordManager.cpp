// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "PasscordManager.h"
#include <iostream>

PasscordManager::PasscordManager()
{
}

PasscordManager::~PasscordManager()
{
}

std::vector<std::pair<int, char>> PasscordManager::passCords;

void PasscordManager::Init()
{
	std::fstream file;
	file.open("C:/Users/remu/Documents/github/VR_2017/Source/VR_2017/passcord.txt");

	if (!file.is_open())
	{
		GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("fail"));
		return;
	}

	while (true)
	{
		int count;
		file >> count;

		if (count == 0)
			break;

		int number;
		char dir;

		for (int i = 0; i < count; ++i)
		{
			file >> number >> dir;
			passCords.emplace_back(number, dir);
		}
	}

	if(passCords.size() != 0)
		GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("ok"));
}
