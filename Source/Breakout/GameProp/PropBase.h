// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshDescription.h"
#include "PropBase.generated.h"


//* Create / replace a section for this procedural mesh component.
//* This function is deprecated for Blueprints because it uses the unsupported 'Color' type.Use new 'Create Mesh Section' function which uses LinearColor instead.
//* @param	SectionIndex		Index of the section to create or replace.
//* @param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
//* @param	Triangles			Index buffer indicating which vertices make up each triangle.Length must be a multiple of 3.
//* @param	Normals				Optional array of normal vectors for each vertex.If supplied, must be same length as Vertices array.
//* @param	UV0					Optional array of texture co - ordinates for each vertex.If supplied, must be same length as Vertices array.
//* @param	VertexColors		Optional array of colors for each vertex.If supplied, must be same length as Vertices array.
//* @param	Tangents			Optional array of tangent vector for each vertex.If supplied, must be same length as Vertices array.
//* @param	bCreateCollision	Indicates whether collision should be created for this section.This adds significant cost.

USTRUCT(BlueprintType)
struct FMeshData
{
	GENERATED_BODY()
	
	FMeshData(TArray<FVector> v = {}, TArray<int32> t = {}, TArray<FVector> n = {}, TArray<FVector2D> u = {}, TArray<FLinearColor> c = {}) : Verts(v), Tris(t), Normals(n), UVs(u), Colors(c) {}
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Verts = {};
	UPROPERTY(BlueprintReadWrite)
	TArray<int32> Tris = {};
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Normals = {};
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector2D> UVs = {};
	UPROPERTY(BlueprintReadWrite)
	TArray<FLinearColor> Colors = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> SectSizes = {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumSections = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Sects = {};

	FORCEINLINE void Clear() {
		Verts = {};
		Tris = {};
		Normals = {};
		UVs = {};
		Colors = {};
		Sects = {};
		SectSizes = {};
		NumSections = 0;
	}

};

UCLASS()
class BREAKOUT_API APropBase : public AActor
{
	GENERATED_BODY()
	
public:	
	APropBase();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void UnifyTri(UPARAM(ref) FMeshData& Data);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Mesh")
	TObjectPtr<class UProceduralMeshComponent> ProceduralMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<class USphereComponent>AreaSphere;


	UPROPERTY(BlueprintReadWrite)
	FMeshData Data1;
	UPROPERTY(BlueprintReadWrite)
	FMeshData Data2;
	UPROPERTY(BlueprintReadWrite)
	FMeshData InterpData;

	float Time = 0.f;
	float MorphingSpeed = 0.3f;
	float Cur = 0.f;
	int32 RandValue;
	double DegSin(double A);

	void InterpMeshData(FMeshData& Data, FMeshData& DataA,FMeshData& DataB, float Alpha, bool Clamp);
	UFUNCTION(BlueprintCallable)
	void GetMeshDataFromStaticMesh(UStaticMesh* Mesh, UPARAM(ref) FMeshData& Data, int32 LODIndex, int32 SectionIndex, bool GetAllSections);
	UFUNCTION(BlueprintCallable)
	void SetColorData(UPARAM(ref) FMeshData& Data, FLinearColor Color);

	FVector CustomLerp(FVector& A, FVector& B, float& Alpha);

	FVector WaveCustomLerp(FVector& A, FVector& B, float& Alpha, float Amplitude, float Frequency);
	FVector SpiralCustomLerp(FVector& A, FVector& B, float& Alpha, float SpiralTurns, float Radius);
	//float Amplitude = 20.0f;
	//float Frequency = 3.0f;
};
