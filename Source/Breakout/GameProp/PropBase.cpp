

#include "GameProp/PropBase.h"
#include "ProceduralMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Character/CharacterBase.h"

//��������
TMap<FMeshData*, TArray<FMeshData>> CachedSections = {};
TMap<FName, FMeshData> MeshDatas = {};

APropBase::APropBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	SetRootComponent(AreaSphere);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetupAttachment(RootComponent);
	
	//�޽� ���� ��������
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMMesh1(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/KA74U/SM_KA74U_X.SM_KA74U_X"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> SMMesh2(TEXT("/Game/FPS_Weapon_Bundle/Weapons/Meshes/SMG11/SM_SMG11_X.SM_SMG11_X"));

	////�޽� �����Ϳ� ����ƽ �޽� ������ �ֱ�
	GetMeshDataFromStaticMesh(SMMesh1.Object, Data1, 0, 0, true);
	GetMeshDataFromStaticMesh(SMMesh2.Object, Data2, 0, 0, true);

	TArray<FProcMeshTangent> Tangents = {};
	ProceduralMesh->CreateMeshSection_LinearColor
	(
		0,  //SECK INDEX
		Data1.Verts,  //V
		Data1.Tris, //T
		Data1.Normals, //N
		Data1.UVs, //UV
		Data1.Colors, //C
		Tangents, //T
		false // COLLISION
	);


}

// Called when the game starts or when spawned
void APropBase::BeginPlay()
{
	Super::BeginPlay();

	if (AreaSphere)
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &APropBase::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &APropBase::OnSphereEndOverlap);
	}


}

void APropBase::InterpMeshData(FMeshData& Data, FMeshData& DataA, FMeshData& DataB, float Alpha, bool Clamp)
{
	int x = 0, l = Data.Verts.Num(), al = DataA.Verts.Num(), bl = DataB.Verts.Num();
	if (l <= 0 || al <= 0 || bl <= 0)
	{
		return; 
	}
	if (Clamp) 
	{
		if (Alpha <= 0.0f) 
		{
			if (Data.Verts[0] != DataA.Verts[0]) { Data = DataA; }
			return;
		}
		if (Alpha >= 1.0f) 
		{
			if (Data.Verts[0] != DataB.Verts[0]) { Data = DataB; }
			return;
		}
	}
	int ml = l = std::min(l, al);
	ml = std::min(ml, bl);
	const bool hasNormals = (Data.Normals.Num() >= ml && DataA.Normals.Num() >= ml && DataB.Normals.Num() >= ml);
	const bool hasUVs = (Data.UVs.Num() >= ml && DataA.UVs.Num() >= ml && DataB.UVs.Num() >= ml);
	const bool hasColors = (Data.Colors.Num() >= ml && DataA.Colors.Num() >= ml && DataB.Colors.Num() >= ml);
	int y = 0;
	for (x = 0; x < l; ++x) 
	{
		y = x;
		if (bl < l && y >= bl) 
		{
			y = (y % bl) / 3; 
		}
		Data.Verts[x] = FMath::Lerp(DataA.Verts[x], DataB.Verts[y], Alpha);
		if (hasNormals) 
		{
			Data.Normals[x] = FMath::Lerp(DataA.Normals[x], DataB.Normals[y], Alpha);
			Data.Normals[x].Normalize();
		}
		if (hasColors) 
		{
			Data.UVs[x] = FMath::Lerp(DataA.UVs[x], DataB.UVs[y], Alpha);
		}
		if (hasColors) 
		{
			Data.Colors[x] = FMath::Lerp(DataA.Colors[x], DataB.Colors[y], Alpha);
		}
	}
}

void APropBase::GetMeshDataFromStaticMesh(UStaticMesh* Mesh, FMeshData& Data, int32 LODIndex, int32 SectionIndex, bool GetAllSections)
{
	int32 n = 0, svi = 0, vi = 0, sec = 0;
	int32* NewIndexPtr = nullptr;
	if (Mesh == nullptr || Mesh->GetRenderData() == nullptr || !Mesh->GetRenderData()->LODResources.IsValidIndex(LODIndex)) 
	{
		return;
	}
	if (!Mesh->bAllowCPUAccess) 
	{
		
	}
	Data.Clear();

	while (true)
	{
		const FStaticMeshLODResources& LOD = Mesh->GetRenderData()->LODResources[LODIndex];
		if (!LOD.Sections.IsValidIndex(SectionIndex)) 
		{
			Data.CountSections(); return; 
		}
		TMap<int32, int32> MeshToSectionVertMap = {};
		uint32 i = 0, is = LOD.Sections[SectionIndex].FirstIndex, l = LOD.Sections[SectionIndex].FirstIndex + LOD.Sections[SectionIndex].NumTriangles * 3;
		FIndexArrayView Indices = LOD.IndexBuffer.GetArrayView();
		uint32 il = Indices.Num();
		const bool hasColors = LOD.VertexBuffers.ColorVertexBuffer.GetNumVertices() >= LOD.VertexBuffers.PositionVertexBuffer.GetNumVertices();
		for (i = is; i < l; ++i) {
			if (i < il) {
				vi = Indices[i];
				NewIndexPtr = MeshToSectionVertMap.Find(vi);
				if (NewIndexPtr != nullptr)
				{ 
					svi = *NewIndexPtr; 
				}
				else 
				{
					Data.Verts.Emplace(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(vi));
					Data.Normals.Emplace(LOD.VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(vi));
					Data.UVs.Emplace(LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(vi, 0));
					Data.Sects.Emplace(sec);
					if (hasColors) 
					{
						Data.Colors.Emplace(LOD.VertexBuffers.ColorVertexBuffer.VertexColor(vi)); 
					}
					svi = n;
					MeshToSectionVertMap.Emplace(vi, n);
					++n;
				}
				Data.Tris.Emplace(svi);
			}

		}

		if (!GetAllSections)
		{
			Data.CountSections(); return;
		}
		SectionIndex += 1;
		sec += 1;
		Data.NumSections += 1;
	}
}

void APropBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ACharacterBase* CharacterBase = Cast<ACharacterBase>(OtherActor);

	//CharacterBase->SetbCanObtainEscapeTool(true);
	//CharacterBase->OverlappingEscapeTool = this;

	////UE_LOG(LogTemp, Log, TEXT("OBTAIN"));
}

void APropBase::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//if (Mesh->bHiddenInGame==false)
	//{
	//	ACharacterBase* CharacterBase = Cast<ACharacterBase>(OtherActor);

	//	CharacterBase->SetbCanObtainEscapeTool(false);
	//	CharacterBase->OverlappingEscapeTool = nullptr;

	//}
}

double APropBase::DegSin(double A)
{
	return FMath::Sin(3.141592/ (180.0) * A);
}

void APropBase::ProceduralMeshFromMeshData(UProceduralMeshComponent* Mesh, FMeshData& Data, int SectionIndex, bool Collision, bool CalcTangents)
{
	if (!Mesh) return;

	TArray<FMeshData> ConvertOutput = ConvertFromSectionedMeshData(Data);
	CachedSections.Emplace(&Data, ConvertOutput);
	TArray<FProcMeshTangent> Tangents = {};
	for (int x = 0; x < ConvertOutput.Num(); ++x)
	{
		FMeshData& r = ConvertOutput[x];
		Mesh->CreateMeshSection_LinearColor(SectionIndex + x, r.Verts, r.Tris, r.Normals, r.UVs, r.Colors, Tangents, Collision);
		return;
	}
	Mesh->CreateMeshSection_LinearColor(SectionIndex, Data.Verts, Data.Tris, Data.Normals, Data.UVs, Data.Colors, Tangents, Collision);
}

TArray<FMeshData> APropBase::ConvertFromSectionedMeshData(FMeshData& Data)
{


	return TArray<FMeshData>();
}



void APropBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Cur = FMath::Clamp
	(
		(((DegSin(Cur * 180.f) * 1.1) + 1.0) / 2.f),
		0.f,
		1.f
	);

	InterpMeshData(InterpData, Data1, Data2, Cur, false);
	ProceduralMesh->UpdateMeshSection_LinearColor(0, InterpData.Verts, InterpData.Normals, InterpData.UVs, InterpData.Colors, TArray<FProcMeshTangent>());
}

void APropBase::SetHideMesh()
{
	//Mesh->bHiddenInGame = true;
}

