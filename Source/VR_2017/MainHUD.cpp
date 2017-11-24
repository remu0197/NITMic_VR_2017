// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "MainHUD.h"

AMainHUD::AMainHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/UI/timeup"));
	timeupTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> finTexObj(TEXT("/Game/UI/fin"));
	finTex = finTexObj.Object;
}


void AMainHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	// �摜�̍��オ(0.0f, 0.0f)
	const FVector2D CrosshairDrawPosition(0.0f, 0.0f);

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, timeupTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

}


