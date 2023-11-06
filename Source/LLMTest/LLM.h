// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "GameFramework/Actor.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "LLM.generated.h"



UCLASS()
class LLMTEST_API ALLM : public AActor
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

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "LLM")
	void SendTextToLLM(FString UserInput);

	void OnResponseReceived(FHttpRequestPtr, FHttpResponsePtr, bool);
	
	void ParseResponse(FString);
};
