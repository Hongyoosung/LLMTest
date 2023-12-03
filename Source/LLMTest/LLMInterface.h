// ILLMInterface.h
#pragma once

#include "CoreMinimal.h"
#include "LLMInterface.generated.h"

UINTERFACE(MinimalAPI)
class ULLMInterface : public UInterface
{
    GENERATED_BODY()
};

class ILLMInterface
{
    GENERATED_BODY()

public:
    virtual void SendTextToLLM(int AgentHealth, int EnemyHealth) = 0;
    virtual void SendTextToLLM() = 0;
    virtual FString GetResponse() const = 0;
    virtual void SetOnResponseReadyCallback(TFunction<void()> Callback) = 0;
    virtual bool IsFirstRequest() = 0;
};
