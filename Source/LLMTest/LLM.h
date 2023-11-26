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

	// ������ �����ϴ� ��� ���� �߰�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LLM")
	FString ResponseContent;

	// ������ ��ȯ�ϴ� �޼��� �߰�
	UFUNCTION(BlueprintCallable, Category = "LLM")
	FString GetResponse() const;

	UFUNCTION(BlueprintCallable, Category = "LLM")
	void SendTextToLLM(int AgentHealth, int EnemyHealth);

	virtual void Tick(float DeltaTime) override;
	virtual void SetOnResponseReadyCallback(TFunction<void()> Callback) override;

	
	void OnResponseReceived(FHttpRequestPtr, FHttpResponsePtr, bool);
	void ParseResponse(FString);


protected:
	virtual void BeginPlay() override;

public:	
	

private:
	TFunction<void()> OnResponseReadyCallback;
};
