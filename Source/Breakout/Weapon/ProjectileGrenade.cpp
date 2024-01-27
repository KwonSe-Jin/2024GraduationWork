// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->InitialSpeed = 500.f;
	DestroyTime = 3.f;
}

void AProjectileGrenade::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectileGrenade::DestroyTimerFinished,
		DestroyTime
	);
}

void AProjectileGrenade::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileGrenade::BeginPlay()
{
	Super::BeginPlay();

	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);

}

//�ٿ�Ҷ����� �߻� ���߿� ���� �߰�
void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
}
