

#include "LLM.h"
#include <Subsystems/PanelExtensionSubsystem.h>
#include "Json.h"
#include "JsonUtilities.h"
#include "HttpModule.h"

#include "Engine/Engine.h"
#include "Engine/World.h"


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

// Example �ڵ带 Unreal Engine ������ �°� ����
// �������Ʈ���� ����� �� �ֵ��� ����, �Էµ� �ؽ�Ʈ�� LLN�� ����
void ALLM::SendTextToLLM(FString UserInput)
{

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    Request->           SetURL(TEXT("http://localhost:1234/v1/chat/completions"));
    Request->           SetVerb(TEXT("POST"));
    Request->           SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    FString JsonData    = FString::Printf(TEXT("{ \"messages\": [ { \"role\": \"user\", \"content\": \"%s\" } ], \"stop\": [\"### Instruction:\"], \"temperature\": 0.7, \"max_tokens\": -1, \"stream\": true }"), *UserInput);

    Request->           SetContentAsString(JsonData);
    Request->           OnProcessRequestComplete().BindUObject(this, &ALLM::OnResponseReceived);

    Request->           ProcessRequest();


    UE_LOG(LogTemp, Warning, TEXT("LLM Connected"));
}

// ������ �޾��� �� ȣ��Ǵ� �޼��� ����
// ������ �Ľ��Ͽ� ����� �� �ֵ���
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
}

// ������ �Ľ��ϴ� �޼��� ����
void ALLM::ParseResponse(FString ResponseR)
{
    TSharedPtr<FJsonObject>     JsonObject;

    TSharedRef<TJsonReader<>>   Reader      = TJsonReaderFactory<>::Create(ResponseR);


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

// ������ ��ȯ�ϴ� �޼��� ����
// �������Ʈ���� ����� �� �ֵ��� ����
FString ALLM::GetResponse() const
{
    return ResponseContent;
}



