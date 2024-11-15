#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StaticMeshList.generated.h"

/**
 * 
 */
UCLASS()
class NIGHTDRIVER_API UStaticMeshList : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> Meshes;
};
