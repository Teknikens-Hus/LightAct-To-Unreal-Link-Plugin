// Fill out your copyright notice in the Description page of Project Settings.


#include "ALightActTransformToUnreal.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AALightActTransformToUnreal::AALightActTransformToUnreal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this actor to be hidden in the game	
	SetActorHiddenInGame(true);
	// Create the text render component
	ObjectNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ObjectNameText"));
	ObjectNameText->SetupAttachment(RootComponent);
	ObjectNameText->SetText(FText::FromString(TEXT("DefaultObjectName")));
}

// Called when the game starts or when spawned
void AALightActTransformToUnreal::BeginPlay()
{
	Super::BeginPlay();
}
	
// Called every frame
void AALightActTransformToUnreal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AALightActTransformToUnreal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FString ActorName = GetName();

	if (RootComponent)
	{
		ObjectNameText->SetText(FText::FromString(ActorName));
		//UE_LOG(LogTemp, Warning, TEXT("Actor %s has begun playing!"), *ActorName);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Actor has no root component!"));
	}
}

void AALightActTransformToUnreal::ValidatePath()
{
	FString TempPath = FilePath;
	if (FPaths::IsRelative(TempPath))
	{
		TempPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir(), TempPath);
	}

	TSharedPtr<FJsonObject> JsonObject;
	if (LoadJsonFromFile(TempPath, JsonObject))
	{
		UE_LOG(LogTemp, Log, TEXT("File is valid! %s"), *TempPath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("File is not valid! %s"), *TempPath);
	}
}

void AALightActTransformToUnreal::MakePathRelative()
{
	if (FPaths::IsRelative(FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("File path is already relative! %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("File path is not relative! %s"), *FilePath);
		// Make the path relative to the project's content folder
		FString ProjectContentFolder = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
		UE_LOG(LogTemp, Log, TEXT("Project content folder is %s"), *ProjectContentFolder);
		UE_LOG(LogTemp, Log, TEXT("File path is now full! %s"), *FilePath);
		if (FPaths::MakePathRelativeTo(FilePath, *ProjectContentFolder))
		{
			UE_LOG(LogTemp, Log, TEXT("File path is now relative! %s"), *FilePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("File path is not relative! %s"), *FilePath);
		}
	}
}

void AALightActTransformToUnreal::LogLightActObjectID()
{
	TArray<TSharedPtr<FJsonValue>> LightActObjects = GetLightActObjects();
	// Iterate through each object
	for (const TSharedPtr<FJsonValue>& ObjectValue : LightActObjects)
	{
		// Access the object's properties
		TSharedPtr<FJsonObject> ObjectData = ObjectValue->AsObject();
		FString ObjectName = ObjectData->GetStringField("Name");
		int32 ObjectID = ObjectData->GetIntegerField("LAId");
		//FVector ObjectPos = VectorFromJsonArray(ObjectData->GetArrayField("Pos"));
		//FVector ObjectRot = VectorFromJsonArray(ObjectData->GetArrayField("Rot"));
		//FVector ObjectSize = VectorFromJsonArray(ObjectData->GetArrayField("Size"));

		// Print out the object's information
		UE_LOG(LogTemp, Log, TEXT("LightAct-Object %s has ID %d"), *ObjectName, ObjectID);
	}
}

TArray<TSharedPtr<FJsonValue>> AALightActTransformToUnreal::GetLightActObjects()
{
	FString TempPath = FilePath;
	if (FPaths::IsRelative(TempPath))
	{
		TempPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir(), TempPath);
	}
	TArray<TSharedPtr<FJsonValue>> EmptyArray;
	TSharedPtr<FJsonObject> JsonObject;
	if (LoadJsonFromFile(TempPath, JsonObject))
	{
		if(bwriteDebugLog)
		{
			UE_LOG(LogTemp, Log, TEXT("Loading file %s"), *TempPath);
		}
		// Access the "Viewport" key
		TSharedPtr<FJsonObject> ViewportObject = JsonObject->GetObjectField("Viewport");
		if (ViewportObject.IsValid())
		{
			// Access the "Objects" array
			TArray<TSharedPtr<FJsonValue>> ObjectsArray = ViewportObject->GetArrayField("Objects");
			return ObjectsArray;
		}
		else
		{
			// Log an error if the "Viewport" key is not found
			UE_LOG(LogTemp, Error, TEXT("Viewport key not found in JSON file"));
			return EmptyArray;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("File is not valid! %s"), *TempPath);
		return EmptyArray;
	}
}

void AALightActTransformToUnreal::CheckMapping() {
	TArray<TSharedPtr<FJsonValue>> LightActObjects = GetLightActObjects();
	// Iterate through each object
	for (const TSharedPtr<FJsonValue>& ObjectValue : LightActObjects)
	{
		// Access the object's properties
		TSharedPtr<FJsonObject> ObjectData = ObjectValue->AsObject();
		FString ObjectName = ObjectData->GetStringField("Name");
		int32 ObjectID = ObjectData->GetIntegerField("LAId");
		//Search the map for the object ID
		FName UnrealActorTag = LightActIdToUnrealActorTagMap.FindRef(ObjectID);
		if (UnrealActorTag.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("Object %s has no mapping!"), *ObjectName);
		}
		else
		{
			FString FormattedLog = FString::Printf(TEXT("Object %s with ID %d has mapping %s"), *ObjectName, ObjectID, *UnrealActorTag.ToString());
			UE_LOG(LogTemp, Log, TEXT("%s"), *FormattedLog);
		}
	}

}

void AALightActTransformToUnreal::UpdateMappedActors() {
	// Get the LightAct objects and the StaticMeshActors with any tag
	TArray<TSharedPtr<FJsonValue>> LightActObjects = GetLightActObjects();
	TArray<AStaticMeshActor*> StaticMeshActorsWithAnyTag = GetStaticMeshActorsWithAnyTag();
	// Since LightAct uses meters and Unreal uses centimeters, we need to scale the location 100 times
	float LocationScale = 100.0f;
	// Check if there are any objects and any actors with any tag
	if(LightActObjects.Num() > 0 && StaticMeshActorsWithAnyTag.Num() > 0)
	{
		// Iterate through each object
		for (const TSharedPtr<FJsonValue>& ObjectValue : LightActObjects)
		{
			// Access the object's properties
			TSharedPtr<FJsonObject> ObjectData = ObjectValue->AsObject();
			int32 ObjectID = ObjectData->GetIntegerField("LAId");
			//Search the map for the object ID
			FName UnrealActorTag = LightActIdToUnrealActorTagMap.FindRef(ObjectID);
			if (!UnrealActorTag.IsNone())
			{
				//Find the actor with the tag
				for (AStaticMeshActor* Actor : StaticMeshActorsWithAnyTag)
				{
					if (Actor->Tags[0] == UnrealActorTag)
					{
						//Set the actor's transform
						FVector ObjectPos = VectorFromJsonArray(ObjectData->GetArrayField("Pos"));
						// The rotation in the json file is in quaternion format, an array with 4 values. So we need to get it as a quaternion
						FQuat ObjectRotQuat = QuatFromJsonArray(ObjectData->GetArrayField("Rot"));
						FVector ObjectSize = VectorFromJsonArray(ObjectData->GetArrayField("Size"));
						//Scale the location
						ObjectPos *= LocationScale;
						// We need to convert LightAct's Y Up System to Unreal's Z Up System
						ObjectPos = FVector(ObjectPos.X, ObjectPos.Z, ObjectPos.Y);
						// Set the actor's transform
						Actor->SetActorLocation(ObjectPos);
						// We need to convert LightAct's Y Up rotation to Unreal's Z Up rotation
						ObjectRotQuat = ConvertQuaternion(ObjectRotQuat);
						Actor->SetActorRotation(ObjectRotQuat);
						// Set the actor's scale
						Actor->SetActorScale3D(ObjectSize);
						if(bwriteDebugLog)
						{
							UE_LOG(LogTemp, Log, TEXT("Updated actor %s with tag %s"), *Actor->GetName(), *Actor->Tags[0].ToString());
						}
						
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No LightAct objects or no StaticMeshActors with any tag!"));
	}

}

FVector AALightActTransformToUnreal::VectorFromJsonArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	// Create a vector from the array
	return FVector(JsonArray[0]->AsNumber(), JsonArray[1]->AsNumber(), JsonArray[2]->AsNumber());
}

FQuat AALightActTransformToUnreal::QuatFromJsonArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray)
{
	if (JsonArray.Num() == 4)
	{
		// Create a quaternion from the array
		return FQuat(
			JsonArray[0]->AsNumber(),
			JsonArray[1]->AsNumber(),
			JsonArray[2]->AsNumber(),
			JsonArray[3]->AsNumber()
		);
	}
	else
	{
		// Log an error if the array doesn't have the expected size
		UE_LOG(LogTemp, Error, TEXT("Invalid quaternion array size."));
		return FQuat::Identity;
	}
}

FQuat AALightActTransformToUnreal::ConvertQuaternion(const FQuat& originalQuaternion)
{
	// Extract axis and angle from the original quaternion
	FVector axis;
	float angle;
	originalQuaternion.ToAxisAndAngle(axis, angle);

	// Coordinate system transformation: Swap Y and Z components
	FVector transformedAxis(axis.X, axis.Z, -axis.Y);

	// Adjust quaternion components for the new coordinate system
	FQuat convertedQuaternion = FQuat(transformedAxis, angle);

	// Adjust handedness if necessary
	convertedQuaternion.Y = -convertedQuaternion.Y;
	convertedQuaternion.Z = -convertedQuaternion.Z;

	// Invert Z-rotation in Unreal Engine
	convertedQuaternion.Z = -convertedQuaternion.Z;

	return convertedQuaternion;
}


TArray<AStaticMeshActor*> AALightActTransformToUnreal::GetStaticMeshActorsWithAnyTag()
{
	// Create an array to store static mesh actors with any tag
	TArray<AStaticMeshActor*> StaticMeshActorsWithAnyTag;
	TArray<AStaticMeshActor*> EmptyArray;
	// Get all actors in the level
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStaticMeshActor::StaticClass(), AllActors);

	// Loop through all actors to find those with any tag
	for (AActor* Actor : AllActors)
	{
		// Check if the actor has any tags
		if (Actor->Tags.Num() > 0)
		{
			// If it's a static mesh actor, add it to the array
			AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
			if (StaticMeshActor)
			{
				StaticMeshActorsWithAnyTag.Add(StaticMeshActor);
			}
		}
	}
	if (StaticMeshActorsWithAnyTag.Num() > 0)
	{
		return StaticMeshActorsWithAnyTag;
	}
	else
	{
		return EmptyArray;
	}
}

void AALightActTransformToUnreal::LogStaticMeshActorsWithAnyTag()
{
	TArray<AStaticMeshActor*> StaticMeshActorsWithAnyTag = GetStaticMeshActorsWithAnyTag();
	for (AStaticMeshActor* Actor : StaticMeshActorsWithAnyTag)
	{
		UE_LOG(LogTemp, Log, TEXT("Actor: %s with tag: %s"), *Actor->GetName(), *Actor->Tags[0].ToString());
	}
}

bool AALightActTransformToUnreal::LoadJsonFromFile(const FString& InFilePath, TSharedPtr<FJsonObject>& OutJsonObject)
{
	// Read the file content into a string
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *InFilePath))
	{
		// Failed to read the file
		return false;
	}

	// Create a JSON reader
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	// Deserialize the JSON
	return FJsonSerializer::Deserialize(JsonReader, OutJsonObject);
}