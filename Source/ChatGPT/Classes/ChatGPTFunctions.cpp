// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatGPTFunctions.h"

FString UChatGPTFunctions::GetCompletionFromResponse(FChatGPTResponse Response, int Index)
{
	if(Response.choices.Num() == 0)
		return FString(TEXT("No response from API"));
	else if (Response.choices.Num() < Index)
		return FString(TEXT("Index out of bounds"));
	return Response.choices[Index].message.content;
}
