// Fill out your copyright notice in the Description page of Project Settings.


#include "GameProp/EscapeTool.h"
#include "Character/CharacterBase.h"
#include "ProceduralMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "HUD/ETPercentBar.h"
AEscapeTool::AEscapeTool()
{
	PercentBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("PercentBar"));
	PercentBar->SetupAttachment(RootComponent);
}

void AEscapeTool::BeginPlay()
{
	Super::BeginPlay();

	if (AreaSphere)
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AEscapeTool::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AEscapeTool::OnSphereEndOverlap);
	}
	PercentBar->SetVisibility(false);

	UpdatePercent(Cur);
}

void AEscapeTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOverlap==2)
	{
		TransformMesh(DeltaTime, false,true);
		UpdatePercent(Cur);
	}



}

void AEscapeTool::TransformMesh(float DeltaTime, bool Clamp, bool TransformReverse)
{

	if (OverlapedCharacter)
	{
		if (Cur >= 1.f)
		{
			//UE_LOG(LogTemp, Log, TEXT("CUR 1.F"));
			OverlapedCharacter->SetbCanObtainEscapeTool(true);
			PercentBar->SetVisibility(false);
			bOverlap = 0;
		}
		else if (Cur <= 0.f)
		{
			bOverlap = 0;
		}
	}

	Cur = FMath::Clamp
	(
		Time/*(((DegSin(Time * 180.f) * 1.1) + 1.0) / 2.f)*/,
		0.f,
		1.f
	);

	InterpMeshData(InterpData, Data1, Data2, Cur, Clamp);

	ProceduralMesh->UpdateMeshSection_LinearColor(0, InterpData.Verts, InterpData.Normals, InterpData.UVs, InterpData.Colors, TArray<FProcMeshTangent>());

	if (TransformReverse)
		Time = Time - (DeltaTime * MorphingSpeed);
	else
		Time = Time + (DeltaTime * MorphingSpeed);
	UpdatePercent(Cur);
}

void AEscapeTool::SetHideMesh()
{
	UE_LOG(LogTemp, Log, TEXT("TEST"));
	ProceduralMesh->SetHiddenInGame(true);
	Destroy();
}

void AEscapeTool::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlapedCharacter = Cast<ACharacterBase>(OtherActor);
	if (OverlapedCharacter)
	{
		OverlapedCharacter->OverlappingEscapeTool = this;
	}
	PercentBar->SetVisibility(true);
	//CharacterBase->SetbCanObtainEscapeTool(true);

//UE_LOG(LogTemp, Log, TEXT("OBTAIN"));
	bOverlap = 1;
}

void AEscapeTool::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ACharacterBase* characterbase = Cast<ACharacterBase>(OtherActor);

	characterbase->SetbCanObtainEscapeTool(false);
	characterbase->OverlappingEscapeTool = nullptr;
	bOverlap = 2;
	
}

void AEscapeTool::UpdatePercent(float Percent)
{
	if (PercentBar->GetWidget())
	{
		Cast<UETPercentBar>(PercentBar->GetWidget())->PercentBar->SetPercent(Percent);
	}
}