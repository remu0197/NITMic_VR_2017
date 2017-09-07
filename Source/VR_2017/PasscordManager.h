// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <fstream>

/**
 * 
 */
class VR_2017_API PasscordManager
{
private:
	PasscordManager();
	~PasscordManager();

	static std::vector<std::pair<int, char>> passCords;

public:
	static void Init();
};
