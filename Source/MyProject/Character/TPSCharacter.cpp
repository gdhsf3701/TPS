// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPSCharacter.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/TPSAnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Weapon.h"
// Sets default values
ATPSCharacter::ATPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;

	//Mesh
	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -88.0));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0))
		;


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/_Art/MilitaryCharDark/MW_Style2_Male.MW_Style2_Male'"));
	if (MeshRef.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshRef.Object);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 500;
	SpringArm->SetRelativeRotation(FRotator(-30, 0, 0));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0, 50, 50);

	GetCharacterMovement()->MaxWalkSpeed = 300;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
#pragma region InputSystem
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCDefaultRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));
	if (IMCDefaultRef.Succeeded())
		IMCDefault = IMCDefaultRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Jump.IA_Jump'"));
	if (JumpActionRef.Succeeded())
		JumpAction = JumpActionRef.Object;


	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Move.IA_Move'"));
	if (MoveActionRef.Succeeded())
		MoveAction = MoveActionRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> TurnActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Turn.IA_Turn'"));
	if (TurnActionRef.Succeeded())
		TurnAction = TurnActionRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Run.IA_Run'"));
	if (RunActionRef.Succeeded())
		RunAction = RunActionRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Fire.IA_Fire'"));
	if (FireActionRef.Succeeded())
		FireAction = FireActionRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/Actions/IA_Reload.IA_Reload'"));
	if (ReloadActionRef.Succeeded())
		ReloadAction = ReloadActionRef.Object;

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Script/Engine.AnimBlueprint'/Game/Animation/ABP_Character.ABP_Character_C'"));
	if (AnimInstanceRef.Succeeded()) 
	{
		GetMesh()->SetAnimClass(AnimInstanceRef.Class);
	}

	static  ConstructorHelpers::FClassFinder<AWeapon> WeaponRef(TEXT("/Script/Engine.Blueprint'/Game/BluePrints/Weapon.Weapon_C'"));
	if (WeaponRef.Succeeded()) {
		WeaponClass = WeaponRef.Class;

	}
#pragma endregion


}

// Called when the game starts or when spawned
void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponClass) {
		AttachWeapon(WeaponClass);
	}
	else {
		AttachWeapon(AWeapon::StaticClass());
	}
	GetCharacterMovement()->MaxWalkSpeed = 300;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (SubSystem) {
			SubSystem->AddMappingContext(IMCDefault, 0);

		}
	}
	
}

// Called every frame
void ATPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhanedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhanedInputComponent)
	{
		EnhanedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhanedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhanedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Move);
		EnhanedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Turn);
		EnhanedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Run);
		EnhanedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ATPSCharacter::Input_Run);
		EnhanedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Fire);
		EnhanedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Reload);
	}
}

void ATPSCharacter::AttachWeapon(TSubclassOf<class AWeapon> NewWeapon)
{
	if (NewWeapon) {
		FActorSpawnParameters PawnParams;
		PawnParams.Owner = this;
		EquipWeapon = GetWorld()->SpawnActor<AWeapon>(NewWeapon, PawnParams);

		const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("WeaponSocket");

		if (WeaponSocket && EquipWeapon) 
		{
			WeaponSocket->AttachActor(EquipWeapon, GetMesh());

		}
	}
}

void ATPSCharacter::StartReloading()
{
	UTPSAnimInstance* AnimInstance = Cast<UTPSAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return;
	if (EquipWeapon == nullptr)
		return;
	AnimInstance->PlayReloadMontage();

	bIsReload = true;
	EquipWeapon->StopFire();
	EquipWeapon->Reloading();

}

void ATPSCharacter::FinishReloading()
{
	UTPSAnimInstance* AnimInstance = Cast<UTPSAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return;

	if (EquipWeapon == nullptr)
		return;

	AnimInstance->StopAllMontages(false);

	bIsReload = false;
	EquipWeapon->FinishReloading();
}

void ATPSCharacter::Input_Move(const FInputActionValue& InputValue)
{
	const FVector2D MovementVector = InputValue.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.X);
	AddMovementInput(GetActorRightVector(), MovementVector.Y);
}

void ATPSCharacter::Input_Turn(const FInputActionValue& InputValue)
{
	const FVector2D LookAxisVector = InputValue.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ATPSCharacter::Input_Run(const FInputActionValue& InputValue)
{
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, TEXT("Run"));
	const bool IsRun = InputValue.Get<bool>();
	if(IsRun == true)
		GetCharacterMovement()->MaxWalkSpeed = 600;
	else
		GetCharacterMovement()->MaxWalkSpeed = 300;

}

void ATPSCharacter::Input_Fire(const FInputActionValue& InputValue)
{

	if (bIsReload == true)
		return;	
	UTPSAnimInstance* AnimInstance = Cast<UTPSAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return;

	bool bFireStart = InputValue.Get<bool>();

	if (bFireStart && EquipWeapon->GetAmmoRemainCount()>0)
	{
		AnimInstance->PlayFireMontage();
		EquipWeapon->StartFire(this);
	}
	else 
	{
		AnimInstance->StopAllMontages(false);

		EquipWeapon->StopFire();
	}

}

void ATPSCharacter::Input_Reload(const FInputActionValue& InputValue)
{
	if (bIsReload == true)
		return;	

	StartReloading();
}

