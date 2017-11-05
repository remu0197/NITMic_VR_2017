// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class VR_2017_API TimeManager
{
public:
	TimeManager();
	~TimeManager();

	static void AddPastTime(float deltaTime);

	static float GetPastTime();

	static bool GetIsClockUp();

private:
	static float pastTime;
	static bool isClockUp;
};
