// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemBase.h"
#include "Sound/SoundCue.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	ItemNiagaraMesh = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileMesh"));
	ItemNiagaraMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBase::Destroyed()
{
	if (ObtainNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ObtainNiagara, GetActorLocation());
	}
	if (ObtainSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ObtainSound, GetActorLocation());
	}
}

