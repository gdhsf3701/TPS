// Fill out your copyright notice in the Description page of Project Settings.


#include "TesTActor.h"

// Sets default values
ATesTActor::ATesTActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	SetRootComponent(Body);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/_Art/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));

	if (BodyMeshRef.Succeeded()) {
		Body->SetStaticMesh(BodyMeshRef.Object);
	}
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));
	Water->SetupAttachment(GetRootComponent());
	Water->SetRelativeLocation(FVector(0.0, 0.0, 132.0));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/_Art/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if (WaterMeshRef.Succeeded()) {
		Water->SetStaticMesh(WaterMeshRef.Object);
	}

}

// Called when the game starts or when spawned
void ATesTActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ATesTActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

