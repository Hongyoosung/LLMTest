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

	// 응답을 저장하는 멤버 변수 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LLM")
	FString ResponseContent;

	// 응답을 반환하는 메서드 추가
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
