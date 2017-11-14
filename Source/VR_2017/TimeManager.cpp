// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "TimeManager.h"

float TimeManager::pastTime = 0.0f;
bool TimeManager::isClockUp = false;

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{
}

void TimeManager::AddPastTime(float deltaTime)
{
	pastTime += deltaTime;
	if (pastTime > 1.0f)
	{
		pastTime = 0.0f;
		isClockUp = true;
		return;
	}

	isClockUp = false;
}

float TimeManager::GetPastTime()
{
	return pastTime;
}

bool TimeManager::GetIsClockUp()
{
	return isClockUp;
}
