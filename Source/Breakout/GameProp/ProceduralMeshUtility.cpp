#include "GameProp/ProceduralMeshUtility.h"
#include "Kismet/KismetMathLibrary.h"

UProceduralMeshUtility::UProceduralMeshUtility()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UProceduralMeshUtility::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProceduralMeshUtility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UProceduralMeshUtility::UnifyTri(MeshData& MeshData)
{
	// �湮�� ������ ����
	TMap<int, int> VisitedVertices = {};
	int CurVertexCount = MeshData.Verts.Num();

	// ���� �Ӽ� ���� üũ
	bool bHasNormals = MeshData.Normals.Num() >= CurVertexCount;
	bool bHasUVs = MeshData.UVs.Num() >= CurVertexCount;
	bool bHasColors = MeshData.Colors.Num() >= CurVertexCount;
	bool bHasSections = MeshData.Sects.Num() >= CurVertexCount;

	for (int TriangleIndex = 0; TriangleIndex < MeshData.Tris.Num(); ++TriangleIndex)
	{
		int VertexIndex = MeshData.Tris[TriangleIndex];

		// �ߺ��� ���� ó��
		if (!VisitedVertices.Contains(VertexIndex))
		{
			VisitedVertices.Emplace(VertexIndex, 1);
		}
		else
		{
			// �ߺ��� ������ ����
			MeshData.Verts.Emplace(MeshData.Verts[VertexIndex]);

			if (bHasNormals)
				MeshData.Normals.Emplace(MeshData.Normals[VertexIndex]);
			if (bHasUVs)
				MeshData.UVs.Emplace(MeshData.UVs[VertexIndex]);
			if (bHasColors)
				MeshData.Colors.Emplace(MeshData.Colors[VertexIndex]);
			if (bHasSections)
				MeshData.Sects.Emplace(MeshData.Sects[VertexIndex]);

			MeshData.Tris[TriangleIndex] = CurVertexCount++;
		}
	}

	// ���� ������ �����صα�
	TArray<FVector> OriginalVerts = MeshData.Verts;
	TArray<FVector> OriginalNormals = MeshData.Normals;
	TArray<FVector2D> OriginalUVs = MeshData.UVs;
	TArray<FLinearColor> OriginalColors = MeshData.Colors;
	TArray<int32> OriginalTris = MeshData.Tris;

	// ���� ������ ������
	MeshData.Verts.Empty();
	MeshData.Tris.Empty();
	MeshData.Normals.Empty();
	MeshData.UVs.Empty();
	MeshData.Colors.Empty();

	for (int TriangleIndex = 0; TriangleIndex < OriginalTris.Num(); ++TriangleIndex)
	{
		MeshData.Verts.Emplace(OriginalVerts[OriginalTris[TriangleIndex]]);

		if (bHasNormals)
			MeshData.Normals.Emplace(OriginalNormals[OriginalTris[TriangleIndex]]);
		if (bHasUVs)
			MeshData.UVs.Emplace(OriginalUVs[OriginalTris[TriangleIndex]]);
		if (bHasColors)
			MeshData.Colors.Emplace(OriginalColors[OriginalTris[TriangleIndex]]);

		MeshData.Tris.Emplace(TriangleIndex);
	}
}

void UProceduralMeshUtility::GetMeshDataFromStaticMesh(UStaticMesh* Mesh, MeshData& Data, int32 LODIndex, int32 SectionIndex, bool GetAllSections)
{
	int32 VertexCount = 0, SectionVertexIndex = 0, VertexIndex = 0, SectionID = 0;

	int32* NewIndexPtr = nullptr;
	if (Mesh == nullptr || Mesh->GetRenderData() == nullptr || !Mesh->GetRenderData()->LODResources.IsValidIndex(LODIndex))
	{
		return;
	}
	Data.Clear();

	while (true)
	{
		// ���� LOD�� ���� ���ҽ��� ������
		const FStaticMeshLODResources& LOD = Mesh->GetRenderData()->LODResources[LODIndex];
		if (!LOD.Sections.IsValidIndex(SectionIndex))
		{
			return;
		}

		// ���ؽ� ������ ���� �� ����
		TMap<int32, int32> MeshToSectionVertMap = {};
		uint32 TriangleIndex = 0;
		uint32	FirstIndex = LOD.Sections[SectionIndex].FirstIndex;
		uint32	LastIndex = FirstIndex + LOD.Sections[SectionIndex].NumTriangles * 3;

		FIndexArrayView Indices = LOD.IndexBuffer.GetArrayView();
		uint32 il = Indices.Num();
		const bool hasColors = LOD.VertexBuffers.ColorVertexBuffer.GetNumVertices() >= LOD.VertexBuffers.PositionVertexBuffer.GetNumVertices();
		for (TriangleIndex = FirstIndex; TriangleIndex < LastIndex; ++TriangleIndex)
		{
			if (TriangleIndex < il)
			{
				VertexIndex = Indices[TriangleIndex];
				NewIndexPtr = MeshToSectionVertMap.Find(VertexIndex);
				if (NewIndexPtr != nullptr)
				{
					// �̹� ���ε� ���ؽ� �ε��� ���
					SectionVertexIndex = *NewIndexPtr;
				}
				else
				{
					// ���ο� ���ؽ� �����͸� ����
					Data.Verts.Emplace(LOD.VertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex));
					Data.Normals.Emplace(LOD.VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex));
					Data.UVs.Emplace(LOD.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0));
					Data.Sects.Emplace(SectionID);
					if (hasColors)
					{
						Data.Colors.Emplace(LOD.VertexBuffers.ColorVertexBuffer.VertexColor(VertexIndex));
					}

					// �� ���ؽ� ���� �߰�
					SectionVertexIndex = VertexCount;
					MeshToSectionVertMap.Emplace(VertexIndex, VertexCount);
					++VertexCount;
				}
				Data.Tris.Emplace(SectionVertexIndex);
			}

		}

		if (!GetAllSections)
		{
			return;
		}
		SectionIndex += 1;
		SectionID += 1;
		Data.NumSections += 1;
	}
}

void UProceduralMeshUtility::SetColorData(MeshData& Data, FLinearColor Color)
{
	Data.Colors = {};
	Data.Colors.SetNumUninitialized(Data.Verts.Num());
	for (int x = 0; x < Data.Verts.Num(); ++x)
	{
		Data.Colors[x] = Color;
	}
}

void UProceduralMeshUtility::InterpMeshData(MeshData& OutData, MeshData& SourceDataA, MeshData& SourceDataB, float Alpha, bool bClamp)
{
	int VertexCount = 0;
	int CurrentVertexCount = OutData.Verts.Num();
	int SourceVertexCountA = SourceDataA.Verts.Num();
	int SourceVertexCountB = SourceDataB.Verts.Num();

	if (CurrentVertexCount <= 0 || SourceVertexCountA <= 0 || SourceVertexCountB <= 0)
	{
		return;
	}

	// Alpha ���� Ŭ���� ������ �ִ��� Ȯ��
	if (bClamp)
	{
		if (Alpha <= 0.0f)
		{
			// Alpha�� 0 ������ ��� SourceDataA ���
			if (OutData.Verts[0] != SourceDataA.Verts[0]) { OutData = SourceDataA; }
			return;
		}
		if (Alpha >= 1.0f)
		{
			// Alpha�� 1 �̻��� ��� SourceDataB ���
			if (OutData.Verts[0] != SourceDataB.Verts[0]) { OutData = SourceDataB; }
			return;
		}
	}

	// ������ �ִ� ���ؽ� �� ����
	int MaxLerpCount = std::min(CurrentVertexCount, std::min(SourceVertexCountA, SourceVertexCountB));
	const bool bHasNormals = (OutData.Normals.Num() >= MaxLerpCount && SourceDataA.Normals.Num() >= MaxLerpCount && SourceDataB.Normals.Num() >= MaxLerpCount);
	const bool bHasUVs = (OutData.UVs.Num() >= MaxLerpCount && SourceDataA.UVs.Num() >= MaxLerpCount && SourceDataB.UVs.Num() >= MaxLerpCount);
	const bool bHasColors = (OutData.Colors.Num() >= MaxLerpCount && SourceDataA.Colors.Num() >= MaxLerpCount && SourceDataB.Colors.Num() >= MaxLerpCount);

	// �� ���ؽ��� ���� ���� ����
	for (int VertexIndex = 0; VertexIndex < CurrentVertexCount; ++VertexIndex)
	{
		int SourceIndexB = VertexIndex;

		// SourceDataB�� �ε����� ��ȿ���� ���� ��� ����
		if (SourceVertexCountB < CurrentVertexCount && SourceIndexB >= SourceVertexCountB)
		{
			SourceIndexB = (SourceIndexB % SourceVertexCountB) / 3;
		}

		OutData.Verts[VertexIndex] = SpiralCustomLerp(SourceDataA.Verts[VertexIndex], SourceDataB.Verts[SourceIndexB], Alpha, 3.f, 30.f);
		if (bHasNormals)
		{
			OutData.Normals[VertexIndex] = SpiralCustomLerp(SourceDataA.Normals[VertexIndex], SourceDataB.Normals[SourceIndexB], Alpha, 3.f, 30.f);
			OutData.Normals[VertexIndex].Normalize();
		}
		if (bHasUVs)
		{
			OutData.UVs[VertexIndex] = FMath::Lerp(SourceDataA.UVs[VertexIndex], SourceDataB.UVs[SourceIndexB], Alpha);
		}
		if (bHasColors)
		{
			OutData.Colors[VertexIndex] = FMath::Lerp(SourceDataA.Colors[VertexIndex], SourceDataB.Colors[SourceIndexB], Alpha);
		}
	}

}

FVector UProceduralMeshUtility::SpiralCustomLerp(FVector& A, FVector& B, float& Alpha, float SpiralTurns, float Radius)
{
	FVector LinearInterpolatedPoint = FMath::Lerp(A, B, Alpha);

	FVector Direction = (B - A).GetSafeNormal();

	FVector OrthogonalVector1 = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();
	FVector OrthogonalVector2 = FVector::CrossProduct(Direction, OrthogonalVector1).GetSafeNormal();

	float Angle = Alpha * SpiralTurns * 2.0f * PI;

	FVector SpiralOffset = (OrthogonalVector1 * FMath::Cos(Angle) + OrthogonalVector2 * FMath::Sin(Angle)) * Radius * (1.0f - Alpha);

	FVector SpiralInterpolatedPoint = LinearInterpolatedPoint + SpiralOffset;

	return SpiralInterpolatedPoint;
}

