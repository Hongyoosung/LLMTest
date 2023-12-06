#include "Agent.h"
#include "NavigationSystem.h"
#include "NavigationSystemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Target.h"



AAgent::AAgent()
{
	Speed = 150.0f;
	SomeSmallValue = 10.0f;
	SomeLargeValue = 500.0f;

	PrimaryActorTick.bCanEverTick = true;

}

void AAgent::BeginPlay()
{
	Super::BeginPlay();

	// Try to get an actor that implements ILLMInterface
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (It->GetClass()->ImplementsInterface(ULLMInterface::StaticClass()))
		{
			LLMInstance = Cast<ILLMInterface>(*It);
			break;
		}
	}

	if (LLMInstance)
	{
		LLMInstance->SetOnResponseReadyCallback([this]() {
			FString Action = LLMInstance->GetResponse();
			UE_LOG(LogTemp, Warning, TEXT("Response Action: %s"), *Action);
			PerformAction(Action);
			});

		// Start the loop by sending the first message to LLM
		PerformActionFromLLM();
	}
}

void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAgent::PerformAction(FString Action)
{
	AActor* Target = UGameplayStatics::GetActorOfClass(GetWorld(), ATarget::StaticClass());

	if (Target)
	{
		if (!Target)
		{
			UE_LOG(LogTemp, Warning, TEXT("Target is not found."));
			return;
		}

		if (Action.Contains("attack") || Action.Contains("ack"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Action: Attack"));
			MoveTowardsTarget(Target);
		}
		else if (Action.Contains("flee") || Action.Contains("lee"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Action: Flee"));
			MoveAwayFromTarget(Target);
		}
		
	}

	// Action has been performed, request next action from LLM
	PerformActionFromLLM();
}


void AAgent::PerformActionFromLLM()
{
	
	AgentHealth = FMath::RandRange(0, 100);
	EnemyHealth = FMath::RandRange(0, 100);

	if (LLMInstance)
	{
		LLMInstance->SendTextToLLM(AgentHealth, EnemyHealth);

	}
	
}



void AAgent::MoveTowardsTarget(AActor* Target)
{

	TargetLocation = Target->GetActorLocation();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAgent::MoveToTargetTick, GetWorld()->GetDeltaSeconds(), true, 0.0f);
}

void AAgent::MoveToTargetTick()
{
	FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
	FVector NewLocation = GetActorLocation() + Direction * Speed * GetWorld()->DeltaTimeSeconds;


	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetLocation);


	if (DistanceToTarget < SomeSmallValue)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		return;
	}

	SetActorLocation(NewLocation);
}

void AAgent::MoveAwayFromTarget(AActor* Target)
{
	TargetLocation = Target->GetActorLocation();
	GetWorld()->GetTimerManager().SetTimer(TimerHandleAway, this, &AAgent::MoveAwayFromTargetTick, GetWorld()->GetDeltaSeconds(), true, 0.0f);
}

void AAgent::MoveAwayFromTargetTick()
{
	FVector Direction = (GetActorLocation() - TargetLocation).GetSafeNormal();
	FVector NewLocation = GetActorLocation() + Direction * Speed * GetWorld()->DeltaTimeSeconds;


	float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetLocation);


	if (DistanceToTarget > SomeLargeValue)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleAway);
		return;
	}

	SetActorLocation(NewLocation);
}