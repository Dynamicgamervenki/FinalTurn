// DynamicV

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "AIController/EnemyAIController.h"

AEnemyAIController::AEnemyAIController()
{
	
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfig_Sight->SightRadius = 750.f;
	AISenseConfig_Sight->LoseSightRadius = 1200.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 30.f;
	AISenseConfig_Sight->SetMaxAge(3.0f);

	AISenseConfig_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("EnemySenseConfig_Hearing");
	AISenseConfig_Hearing->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Hearing->DetectionByAffiliation.bDetectFriendlies = true;
	AISenseConfig_Hearing->DetectionByAffiliation.bDetectNeutrals = true;
	AISenseConfig_Hearing->HearingRange = 1100;
	AISenseConfig_Hearing->SetMaxAge(3.0f);

	
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	AIPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	AIPerceptionComponent->ConfigureSense(*AISenseConfig_Hearing);
	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());

	SetGenericTeamId(FGenericTeamId(1));
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	if (PawnToCheck)
	{
		const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(PawnToCheck->GetController());

		if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() != GetGenericTeamId())
		{
			return ETeamAttitude::Hostile;
		}
	}
	return ETeamAttitude::Friendly;
}