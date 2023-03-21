#include "ChatGPTAPI.h"
#include "JsonUtilities.h"

UChatGPTAPI* UChatGPTAPI::RequestChatGPTResponse(UObject* WorldContextObject, const FChatGPTRequest& Request, const FString& ApiKey)
{
    UChatGPTAPI* ChatGPTAPI = NewObject<UChatGPTAPI>();
    ChatGPTAPI->WorldContextObject = WorldContextObject;
    ChatGPTAPI->ExecuteRequest(Request, ApiKey);
    return ChatGPTAPI;
}

void UChatGPTAPI::ExecuteRequest(const FChatGPTRequest& Request, const FString& ApiKey)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));

    FString RequestBody;
    TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(Request.ToJson().ToSharedRef(), JsonWriter);

    HttpRequest->SetContentAsString(RequestBody);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UChatGPTAPI::HandleResponse);
    HttpRequest->ProcessRequest();
}

void UChatGPTAPI::HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        FString ResponseString = Response->GetContentAsString();
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
        
        if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
        {
            FChatGPTResponse ChatGPTResponse;
            FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ChatGPTResponse, 0, 0);
            OnSuccess.Broadcast(ChatGPTResponse);
        }
        else
        {
            OnError.Broadcast(TEXT("Failed to parse JSON response."));
        }
    }
    else
    {
        FString ErrorMessage = Response.IsValid() ? FString::FromInt(Response->GetResponseCode()) : TEXT("Request failed");
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: %s"), *ErrorMessage));
        OnError.Broadcast(ErrorMessage);
    }
}
