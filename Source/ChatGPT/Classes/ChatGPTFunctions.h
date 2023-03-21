// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChatGPTAPI.h"
#include "UObject/Object.h"
#include "ChatGPTFunctions.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CHATGPT_API UChatGPTFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="ChatGPT")
	static FString GetCompletionFromResponse(FChatGPTResponse Response, int Index);
};
