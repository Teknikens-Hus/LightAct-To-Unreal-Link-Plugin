// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Dom/JsonObject.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "ALightActTransformToUnreal.generated.h"

UCLASS()
class LIGHTACTOBJECTLINK_API AALightActTransformToUnreal : public AActor
{

	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AALightActTransformToUnreal();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObjectDisplay", meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* ObjectNameText;

	UFUNCTION(CallInEditor, Category = "LightAct project")
		void ValidatePath();
	UFUNCTION(CallInEditor, Category = "LightAct project")
		void MakePathRelative();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LightAct project")
		FString FilePath;
	UFUNCTION(CallInEditor, Category = "Unreal Engine Level Actors")
		void LogStaticMeshActorsWithAnyTag();
	UFUNCTION(CallInEditor, Category = "Mapping")
		void CheckMapping();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
		TMap<int32, FName> LightActIdToUnrealActorTagMap;
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Mapping")
		void UpdateMappedActors();
	UFUNCTION(CallInEditor, Category = "LightAct project")
		void LogLightActObjectID();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	bool LoadJsonFromFile(const FString& FilePath, TSharedPtr<FJsonObject>& OutJsonObject);
	FVector VectorFromJsonArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray);
	FQuat QuatFromJsonArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray);
	TArray<TSharedPtr<FJsonValue>> GetLightActObjects();
	TArray<AStaticMeshActor*> GetStaticMeshActorsWithAnyTag();
	FQuat ConvertQuaternion(const FQuat& originalQuaternion);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
