// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EFireType :uint8 {
	EF_Projectile	UMETA(DisplayName = "Projectile"),
	EF_LineTrace	UMETA(DisplayName = "LineTrace"),
};

UCLASS()
class MYPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	virtual void StartFire(TWeakObjectPtr<class ATPSCharacter> OwnerCharacter);
	virtual void StopFire();
	virtual void Reloading();
	virtual void FinishReloading();


protected:
	void FireWithProjectile(TWeakObjectPtr<class ATPSCharacter> OwnerCharacter);
	void FireWithLineTrace(TWeakObjectPtr<class ATPSCharacter> OwnerCharacter);
	void PlayHitEffect(FTransform HitTransform);

public:



	FORCEINLINE int32 GetAmmoMaxCount() {return AmmoMaxCount;}
	FORCEINLINE int32 GetAmmoRemainCount() {return AmmoRemainCount;}
	FORCEINLINE float GetReroadDelayTime() {return ReloadingDelayTime;}

	FORCEINLINE void SetAmmoRemainCount(int32 NewAmmoRemainCount) 
{
		AmmoRemainCount = NewAmmoRemainCount;
	}
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = Properties)
	int AmmoMaxCount = 30;

	UPROPERTY(EditAnywhere, Category = Properties)
	int AmmoRemainCount;

	UPROPERTY(EditAnywhere, Category = Properties)
	float ReloadingDelayTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = Properties)
	float TraceDistance = 5000.0f;

	UPROPERTY(EditAnywhere, Category = Properties)
	float FireInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = HitEffect)
	TObjectPtr<class UParticleSystem> HitEffect;

	UPROPERTY(EditAnywhere, Category = Properties)
	float AttackDamage = 30;

	UPROPERTY(EditAnywhere, Category = Properties)
	EFireType FireType = EFireType::EF_Projectile;
	

private:
	FTimerHandle FireTimerHandle;

};
