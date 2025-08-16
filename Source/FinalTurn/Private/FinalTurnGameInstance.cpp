// DynamicV


#include "FinalTurnGameInstance.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void UFinalTurnGameInstance::LoadGunAsync()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	USkeletalMeshComponent* Mesh = PlayerCharacter->GetMesh();
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		ShotGun.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([this,PlayerCharacter,Mesh]()
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = Cast<AActor>(PlayerCharacter);
			SpawnParams.Instigator = Cast<APawn>(PlayerCharacter);

			FVector SpawnLocation = Mesh->GetSocketLocation(FName("GunUnequipSocket")); 
			FRotator SpawnRotation = Mesh->GetSocketRotation(FName("GunUnequipSocket"));
			UClass* ShotGunClass = ShotGun.Get();
			
			if (ShotGunClass)
			{
				AActor* Gun = GetWorld()->SpawnActor<AActor>(ShotGunClass,SpawnLocation,SpawnRotation,SpawnParams);

				if (Gun)
				{
					Gun->AttachToComponent(Mesh,FAttachmentTransformRules::SnapToTargetIncludingScale,FName("GunUnequipSocket"));
					OnGunSpawned.Broadcast(Gun);
				}
			}
		})
	);

}