// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UObject/Object.h"
#include "Http.h"
#include "HttpModule.h"

#include "ChatGPTAPI.generated.h"

UENUM(BlueprintType)
enum class EChatGPTMessageRole : uint8
{
	assistant,
	user,
	system
};

USTRUCT(BlueprintType)
struct FChatGPTTokenUsage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 count;
};

USTRUCT(BlueprintType)
struct FChatGPTMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EChatGPTMessageRole role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString content;
};

USTRUCT(BlueprintType)
struct FChatGPTChoice
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 index;

	UPROPERTY(BlueprintReadWrite)
	FChatGPTMessage message;

	UPROPERTY(BlueprintReadWrite)
	FString finish_reason;
};

USTRUCT(BlueprintType)
struct FChatGPTUsage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FChatGPTTokenUsage prompt_tokens;

	UPROPERTY(BlueprintReadWrite)
	FChatGPTTokenUsage completion_tokens;

	UPROPERTY(BlueprintReadWrite)
	FChatGPTTokenUsage total_tokens;
};


USTRUCT(BlueprintType)
struct FChatGPTResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString id;

	UPROPERTY(BlueprintReadWrite)
	FString object;

	UPROPERTY(BlueprintReadWrite)
	int64 created;

	UPROPERTY(BlueprintReadWrite)
	TArray<FChatGPTChoice> choices;

	UPROPERTY(BlueprintReadWrite)
	FChatGPTUsage usage;
};

USTRUCT(BlueprintType)
struct FChatGPTRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString model = "gpt-3.5-turbo";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChatGPTMessage> messages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"), Category = "Optional")
	float temperature = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"), Category = "Optional")
	float top_p = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optional")
	int32 n = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optional")
	TArray<FString> stop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optional")
	int32 max_tokens = 128;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "-2", ClampMax = "2", UIMin = "-2", UIMax = "2"), Category = "Optional")
	float presence_penalty = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = "-2", ClampMax = "2", UIMin = "-2", UIMax = "2"), Category = "Optional")
	float frequency_penalty = 0;

	TSharedPtr<FJsonObject> ToJson() const
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

		JsonObject->SetStringField(TEXT("model"), model);

		TArray<TSharedPtr<FJsonValue>> MessagesJsonArray;
		for (const FChatGPTMessage& Message : messages)
		{
			TSharedPtr<FJsonObject> MessageJsonObject = MakeShareable(new FJsonObject);
			MessageJsonObject->SetStringField(TEXT("role"), EnumToString<EChatGPTMessageRole>(TEXT("EChatGPTMessageRole"), Message.role));
			MessageJsonObject->SetStringField(TEXT("content"), Message.content);
			MessagesJsonArray.Add(MakeShareable(new FJsonValueObject(MessageJsonObject)));
		}

		JsonObject->SetArrayField(TEXT("messages"), MessagesJsonArray);

		JsonObject->SetNumberField(TEXT("temperature"), temperature);
		JsonObject->SetNumberField(TEXT("top_p"), top_p);
		JsonObject->SetNumberField(TEXT("n"), n);

		if (stop.Num() > 0)
		{
			TArray<TSharedPtr<FJsonValue>> StopJsonArray;
			for (const FString& StopString : stop)
			{
				StopJsonArray.Add(MakeShareable(new FJsonValueString(StopString)));
			}
			JsonObject->SetArrayField(TEXT("stop"), StopJsonArray);
		}

		JsonObject->SetNumberField(TEXT("max_tokens"), max_tokens);
		JsonObject->SetNumberField(TEXT("presence_penalty"), presence_penalty);
		JsonObject->SetNumberField(TEXT("frequency_penalty"), frequency_penalty);

		return JsonObject;
	}

	template<typename TEnum>
	FString EnumToString(const TCHAR* EnumName, TEnum Value) const
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, EnumName, true);
		if (!EnumPtr)
		{
			return FString("Invalid Enum");
		}
		return EnumPtr->GetNameStringByIndex(static_cast<int64>(Value));
	}
};

/**
 * 
 */
UCLASS()
class UChatGPTAPI : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="Chat GPT API")
	static UChatGPTAPI* RequestChatGPTResponse(UObject* WorldContextObject, const FChatGPTRequest& Request, const FString& ApiKey);
 
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatGPTOnSuccess, const FChatGPTResponse&, Response);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChatGPTOnError, const FString&, ErrorMessage);

	UPROPERTY(BlueprintAssignable)
	FChatGPTOnSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FChatGPTOnError OnError;

protected:
	void ExecuteRequest(const FChatGPTRequest& Request, const FString& ApiKey);
	void HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY()
	UObject* WorldContextObject;

public:
	
};