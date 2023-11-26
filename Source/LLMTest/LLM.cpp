#include "LLM.h"
#include <Subsystems/PanelExtensionSubsystem.h>
#include "Json.h"
#include "JsonUtilities.h"
#include "HttpModule.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>

ALLM::ALLM()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ALLM::BeginPlay()
{
    Super::BeginPlay();
}

void ALLM::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALLM::SendTextToLLM(int AgentHealth, int EnemyHealth)
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    Request->SetURL(TEXT("http://localhost:1234/v1/chat/completions"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    FString JsonData = FString::Printf(TEXT("{ \"messages\": [ { \"role\": \"user\", \"content\": \"You are a game agent that must survive in a given situation and attack enemies. You must choose one of the available actions based on the given state variables. Currently, your health is %d and the enemy's health is %d. Your two available actions are 'flee' and 'attack'. Given your health and the enemy's health, choose which of these two actions is appropriate in the current situation and answer it. However, your answer must use only one word, either 'flee' or 'attack'. No other words or sentences are allowed. Which action do you think is most appropriate in this situation? Say only one of the words 'flee' or 'attack'.\" } ], \"stop\": [\"### Instruction:\"], \"temperature\": 0, \"max_tokens\": 20, \"stream\": false }"), AgentHealth, EnemyHealth);

    Request->SetContentAsString(JsonData);
    Request->OnProcessRequestComplete().BindUObject(this, &ALLM::OnResponseReceived);

    Request->ProcessRequest();


}

void ALLM::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (Request->GetStatus() == EHttpRequestStatus::Succeeded)
    {
        FHttpResponsePtr Response = Request->GetResponse();

        if (Response.IsValid())
        {
            ParseResponse(Response->GetContentAsString());

        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid response"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Request failed"));
    }

    if (OnResponseReadyCallback)
    {
        OnResponseReadyCallback();
    }
}



void ALLM::ParseResponse(FString ResponseR)
{
    TSharedPtr<FJsonObject> JsonObject;

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseR);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        if (JsonObject->HasField("choices"))
        {
            TArray<TSharedPtr<FJsonValue>> Choices = JsonObject->GetArrayField("choices");

            if (Choices.Num() > 0)
            {
                TSharedPtr<FJsonObject> MessageObject = Choices[0]->AsObject()->GetObjectField("message");

                if (MessageObject.IsValid() && MessageObject->HasField("content"))
                {
                    FString Response = MessageObject->GetStringField("content");

                    UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *Response);

                    ResponseContent = Response;
                }
            }
        }
    }
}

void ALLM::SetOnResponseReadyCallback(TFunction<void()> Callback)
{
    OnResponseReadyCallback = Callback;
}

FString ALLM::GetResponse() const
{
    return ResponseContent;
}
