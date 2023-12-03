// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "GameFramework/Actor.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Agent.h"
#include "LLMInterface.h"
#include "LLM.generated.h"



UCLASS()
class LLMTEST_API ALLM : public AActor, public ILLMInterface
{
	GENERATED_BODY()
	
public:	
	ALLM();

	// 응답을 저장하는 멤버 변수 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LLM")
	FString ResponseContent;

	// 응답을 반환하는 메서드 추가
	UFUNCTION(BlueprintCallable, Category = "LLM")
	FString GetResponse() const;

	UFUNCTION(BlueprintCallable, Category = "LLM")
	void SendTextToLLM(int AgentHealth, int EnemyHealth);
	void SendTextToLLM();

	virtual void Tick(float DeltaTime) override;
	virtual void SetOnResponseReadyCallback(TFunction<void()> Callback) override;

	
	void OnResponseReceived(FHttpRequestPtr, FHttpResponsePtr, bool);
	void ParseResponse(FString);
	bool IsFirstRequest();


protected:
	virtual void BeginPlay() override;

public:	
	bool bFirstRequest;

private:
	TFunction<void()> OnResponseReadyCallback;
};
