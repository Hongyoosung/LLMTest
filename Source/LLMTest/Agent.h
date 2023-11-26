// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LLMInterface.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Agent.generated.h"


UCLASS()
class LLMTEST_API AAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	AAgent();

	virtual void Tick(float DeltaTime) override;
	void PerformAction(FString Action);
	void PerformActionFromLLM();
	void MoveTowardsTarget(AActor* Target);
	void MoveAwayFromTarget(AActor* Target);

	UFUNCTION()
	void MoveToTargetTick();

	UFUNCTION()
	void MoveAwayFromTargetTick();

protected:
	virtual void BeginPlay() override;

public:


private:
	ILLMInterface* LLMInstance;
	FTimerHandle LLMHandle;
	FTimerHandle TimerHandle;
	FTimerHandle TimerHandleAway;
	FVector TargetLocation;
	float Speed;
	int SomeSmallValue;
	int SomeLargeValue;
	int AgentHealth;
	int EnemyHealth;

};
