// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameProp/PropBase.h"
#include "EscapeTool.generated.h"

/**
 * 
 */
UCLASS()
class BREAKOUT_API AEscapeTool : public APropBase
{
	GENERATED_BODY()
	
public:
	AEscapeTool();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime);

	void TransformMesh(float DeltaTime, bool Clamp, bool TransformReverse);
	void SetHideMesh();
private:
	UFUNCTION()
	virtual void OnSphereOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	TObjectPtr<class ACharacterBase> OverlapedCharacter;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> PercentBar;

	void UpdatePercent(float Percent);

	//0 = �⺻����
	//1 = a->b�� �������� ����
	//2 = b->a�� �ٽ� ���ư��� ����
	//�̳�Ŭ������ �ٽ� ����� 
	int32 bOverlap=0;
};