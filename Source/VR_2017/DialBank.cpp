// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "DialBank.h"
#include "PasscordManager.h"

ADialBank::ADialBank():
	m_isOperateDial(false)
{
	//passCords.emplace_back(1, 1);
}

void ADialBank::BeginPlay()
{

}

void ADialBank::Tick(float deltaTime)
{

}

ItemName ADialBank::Event(const int innerProduct)
{
	return ItemName::bank;
}


