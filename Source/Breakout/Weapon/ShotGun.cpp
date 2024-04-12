// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ShotGun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Character/CharacterBase.h"
#include "ClientSocket.h"
#include "DrawDebugHelpers.h"
#include "Game/BOGameInstance.h"
#include "GameProp/BulletHoleWall.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void AShotGun::Fire(const FVector& HitTarget)
{
	//AWeaponBase::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	//���� ���ۿ� ������--------------------------------
	TArray<FVector> ServerImpactLoc;
	TArray<FRotator> ServerImpactRot;
	TArray<FVector> ServerBeamStart;
	TArray<FVector> ServerBeamEnd;
	int32 j = 0;
	//---------------------------------------------------

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		TMap<ACharacterBase*, uint32> HitMap;
		TMap<ABulletHoleWall*, uint32> HitWall;
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			/*	FVector End = TraceEndWithScatter(Start, HitTarget);*/
			FHitResult FireHit;
			FVector EndBeamLoc;
			ShotGunTraceHit(Start, HitTarget, FireHit, EndBeamLoc);
			//WeaponTraceHit(Start, HitTarget, FireHit);
			ServerBeamStart.Add(Start);
			ServerBeamEnd.Add(EndBeamLoc);
			ACharacterBase* CharacterBase = Cast<ACharacterBase>(FireHit.GetActor());
			ABulletHoleWall* DamagedWall = Cast<ABulletHoleWall>(FireHit.GetActor());
			if (CharacterBase && HasAuthority() && InstigatorController)
			{
				if (HitMap.Contains(CharacterBase))
				{
					HitMap[CharacterBase]++;
				}
				else
				{
					HitMap.Emplace(CharacterBase, 1);
				}
			}
			else if (DamagedWall)
			{
				DamagedWall->SetBulletHole(FireHit.ImpactPoint);
			}

			if (ImpactNiagara && FireHit.bBlockingHit)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation
				(
					GetWorld(),
					ImpactNiagara,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
				ServerImpactLoc.Add(FireHit.ImpactPoint);
				ServerImpactRot.Add(FireHit.ImpactNormal.Rotation());
			}

		}
		for (int i = 0; i < 10; ++i)
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerBeamEnd : %s"), *ServerBeamEnd[i].ToString());
		}
		Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_packet(Cast<ACharacterBase>(GetOwner())->_SessionId, ServerBeamStart, ServerBeamEnd, ServerBeamStart.Num());

		if (ServerImpactRot.Num() > 0)
		{
			// ����Ʈ ���̾ư��� -> �迭�� ũ��� �׻�5���ƴ� �Ѿ��� �´�Ƚ����ŭ

		}
		//���� ������ ���� ����
		TArray<ACharacterBase*> DamagedCh;
		DamagedCh.SetNum(3);
		DamagedCh[0] = nullptr;
		DamagedCh[1] = nullptr;
		DamagedCh[2] = nullptr;
		TArray<int32> DamageNum;
		DamageNum.SetNum(3);
		DamageNum[0] = 0;
		DamageNum[1] = 0;
		DamageNum[2] = 0;
		int i = 0;
		for (auto HitPair : HitMap)
		{
			++i;
			if (HitPair.Key && HasAuthority() && InstigatorController)
			{
				DamagedCh[i] = HitPair.Key;
				DamageNum[i] = HitPair.Value;
				UGameplayStatics::ApplyDamage
				(
					HitPair.Key,
					Damage * HitPair.Value,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
		}
		// ���� ������
		// ��� �������޾�����
		if (DamagedCh[0] && DamagedCh[1] && DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(DamagedCh[0]->_SessionId, DamagedCh[1]->_SessionId, DamagedCh[2]->_SessionId
				, DamageNum[0]*Damage, DamageNum[1] * Damage, DamageNum[2] * Damage);
		}
		else if(DamagedCh[0] && DamagedCh[1] && !DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(DamagedCh[0]->_SessionId, DamagedCh[1]->_SessionId, -1
				, DamageNum[0] * Damage, DamageNum[1] * Damage, 0);
		}
		else if (DamagedCh[0] && !DamagedCh[1] && DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(DamagedCh[0]->_SessionId, -1, DamagedCh[2]->_SessionId
				, DamageNum[0] * Damage, 0, DamageNum[2] * Damage);
		}
		else if (!DamagedCh[0] && DamagedCh[1] && DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(-1, DamagedCh[1]->_SessionId, DamagedCh[2]->_SessionId
				,0, DamageNum[1] * Damage, DamageNum[2] * Damage);
		}
		else if (!DamagedCh[0] && !DamagedCh[1] && DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(-1, -1, DamagedCh[2]->_SessionId
				, 0, 0, DamageNum[2] * Damage);
		}
		else if (!DamagedCh[0] && DamagedCh[1] && !DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(-1, DamagedCh[1]->_SessionId, -1
				, 0, DamageNum[1] * Damage, 0);
		}
		else if (DamagedCh[0] && !DamagedCh[1] && !DamagedCh[2])
		{
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(DamagedCh[0]->_SessionId, -1, -1
				, DamageNum[0] * Damage, 0, 0);
		}
		/*Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_ShotGun_damaged_packet(DamagedCh[0]->_SessionId);*/
	}
}

void AShotGun::ShotGunTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit, FVector& EndBeamLoc)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;

		World->LineTraceSingleByChannel
		(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		//DrawDebugLine
		//(
		//	World,
		//	TraceStart,
		//	End,
		//	FColor::Cyan,
		//	false,
		//	0.5f
		//);

		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamNiagara)
		{
			StartBeam = TraceStart;
			EndBeam = BeamEnd;
			UNiagaraComponent* Beam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World,
				BeamNiagara,
				TraceStart,
				WeaponMesh->GetComponentRotation(),
				FVector(1.f),
				true
			);

			if (Beam)
			{
				Beam->SetVectorParameter(FName("End"), BeamEnd);
			}
		}
		EndBeamLoc = EndBeam;
	}


}
