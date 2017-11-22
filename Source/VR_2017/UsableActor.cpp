// Fill out your copyright notice in the Description page of Project Settings.

#include "VR_2017.h"
#include "unordered_map"
#include "UsableActor.h"


// Sets default values
AUsableActor::AUsableActor():
	m_itemName(ItemName::noItem)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));

	bReplicates = true;
}

// Called when the game starts or when spawned
void AUsableActor::BeginPlay()
{
	Super::BeginPlay();
	for (UActorComponent* Comp : GetComponentsByClass(UMeshComponent::StaticClass()))
	{
		UMeshComponent* Mesh = Cast<UMeshComponent>(Comp);
		Meshs.Push(Mesh);
	}
}

// Called every frame
void AUsableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FName AUsableActor::GetItemFName()
{
	std::unordered_map<ItemName, FName> myMap{
		{ ItemName::book, "book" },
		{ ItemName::key, "key" },
		{ ItemName::paper, "paper" },
		{ ItemName::clip, "clip" }
	};

	return myMap.at(m_itemName);
}

ItemName AUsableActor::Event(const int innerProduct)
{
	return m_itemName;
}

void AUsableActor::StartFocus()
{
	for (UMeshComponent* Mesh : Meshs)
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->CustomDepthStencilValue = 252;
	}
}

void AUsableActor::EndFocus()
{
	for (UMeshComponent* Mesh : Meshs)
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

