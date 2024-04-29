#include "LLM.h"
#include <Subsystems/PanelExtensionSubsystem.h>
#include "Json.h"
#include "HttpModule.h"
#include "JsonUtilities.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>

ALLM::ALLM()
{
    PrimaryActorTick.bCanEverTick = true;
    bFirstRequest = true;
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
    //UE_LOG(LogTemp, Warning, TEXT("Agent Health: %d, Enemy Health: %d"), AgentHealth, EnemyHealth);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    // OpenAI의 API 서버 URL로 변경.
    Request->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));

    Request->SetVerb(TEXT("POST"));

    // OpenAI API 키를 헤더에 추가.
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), TEXT("Bearer"));

    FString JsonData;
    

    JsonData = FString::Printf(TEXT("{ \"model\": \"gpt-3.5-turbo\", \"messages\": [{ \"role\": \"system\", \"content\": \"You are a game agent, and you are given a set of state variables and possible actions in a given environment. Complete the goal by combining the possible action sets according to the state variables. For example, if the goal is 'attack' and the possible actions are 'equip gun', 'shoot gun', and 'aim at enemy', and the state variable is 'have gun', the action sequence should be 'equip gun' -> 'aim at enemy' -> 'shoot gun'. Now, given the state variables and action sets, complete the possible action sequences and answer the question. Current goal: attack; Current state variables : 'I have a sword', 'I have enough health', 'I see an enemy'; Action set : 'equip sword', 'swing sword', 'approach enemy'\" }], \"max_tokens\": 40 }"), AgentHealth, EnemyHealth);


    Request->SetContentAsString(JsonData);
    Request->OnProcessRequestComplete().BindUObject(this, &ALLM::OnResponseReceived);

    Request->ProcessRequest();
}

void ALLM::SendTextToLLM()
{
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

    // OpenAI의 API 서버 URL로 변경.
    Request->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
    Request->SetVerb(TEXT("POST"));

    // OpenAI API 키를 헤더에 추가.
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), TEXT("Bearer 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"));

    FString JsonData;
    
    JsonData = FString::Printf(TEXT("{ \"model\": \"gpt-3.5-turbo\", \"messages\": [{ \"role\": \"system\", \"content\": \"You are a game agent, given a set of state variables and possible actions in a given environment. Depending on the state variables, you must respond by choosing one of the possible actions. For example, if the possible actions are 'attack' and 'run', you must judge the given state variables and respond with only one word: 'attack' or 'run'. Take the appropriate action to defeat the enemy or survive. So, you are given a set of actions and state variables. If the enemy has more health than the agent, perform 'flee'; if the enemy has less health than the agent, perform 'attack'.\" }], \"max_tokens\": 20 }"));

    
    bFirstRequest = false;

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
        UE_LOG(LogTemp, Error, TEXT("Request failed with status: %s"));
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

    if (!FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response"));
        return;
    }

    if (!JsonObject->HasField("choices"))
    {
        UE_LOG(LogTemp, Error, TEXT("Missing 'choices' field in JSON response"));
        return;
    }

    TArray<TSharedPtr<FJsonValue>> Choices = JsonObject->GetArrayField(TEXT("choices"));

    // 'choices' 안의 각 객체를 순회.
    for (TSharedPtr<FJsonValue> Choice : Choices)
    {
        TSharedPtr<FJsonObject> ChoiceObject = Choice->AsObject();

        // 각 객체에서 'message' 객체를 찾는다.
        if (ChoiceObject->HasField("message"))
        {
            TSharedPtr<FJsonObject> Message = ChoiceObject->GetObjectField("message");

            // 'role'이 'assistant'인 메시지를 찾는다.
            if (Message->GetStringField("role").Equals("assistant"))
            {
                // 'assistant' 메시지에서 'content' 필드를 찾는다.
                FString Content = Message->GetStringField("content").TrimStartAndEnd();

                // 줄바꿈 문자를 제거한다.
                Content = Content.Replace(TEXT("\n"), TEXT(""));

                    ResponseContent = Content;

                // 'content' 필드의 값을 출력한다.
                //UE_LOG(LogTemp, Warning, TEXT("Content: %s"), *Content);
            }
        }
    }
}



bool ALLM::IsFirstRequest()
{
	return bFirstRequest;
}

void ALLM::SetOnResponseReadyCallback(TFunction<void()> Callback)
{
    OnResponseReadyCallback = Callback;
}

FString ALLM::GetResponse() const
{
    return ResponseContent;
}
