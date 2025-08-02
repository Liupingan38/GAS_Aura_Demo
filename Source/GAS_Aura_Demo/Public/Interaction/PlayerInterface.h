// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAS_AURA_DEMO_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToLevel(int32 InLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoint(int32 InAttributePoint);
	
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoint(int32 InSpellPoint);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 InLevel) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 InLevel) const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoint() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoint() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const;
};
