#include "NPC_Chatbot_DemoCharacter.h"
#include "NPC_Chatbot_DemoProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NPC.h"
#include "PersonaComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableTextBox.h"
#include "Components/MultilineEditableTextBox.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "CoreTypes.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANPC_Chatbot_DemoCharacter


ANPC_Chatbot_DemoCharacter::ANPC_Chatbot_DemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ANPC_Chatbot_DemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ANPC_Chatbot_DemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult = PerformLineTrace(300.f);

	ANPC* NPC = Cast<ANPC>(HitResult.GetActor());
	if (NPC)
	{
		isLookingAtItem = true;
	}
	else
	{
		isLookingAtItem = false;
	}

}

//Input

void ANPC_Chatbot_DemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANPC_Chatbot_DemoCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANPC_Chatbot_DemoCharacter::Look);

		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ANPC_Chatbot_DemoCharacter::Interact);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ANPC_Chatbot_DemoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ANPC_Chatbot_DemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

FHitResult ANPC_Chatbot_DemoCharacter::PerformLineTrace(float Distance)
{
	FHitResult HitResult;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	FVector Direction = FirstPersonCameraComponent->GetForwardVector();
	FVector End = Start + (Direction * Distance);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
	{
		return HitResult;
	}
	return FHitResult();
}

void ANPC_Chatbot_DemoCharacter::Interact()
{
	if (FirstPersonCameraComponent != nullptr)
	{
		FHitResult NPCHitResult = PerformLineTrace(300.f);
		InteractingNPC = Cast<ANPC>(NPCHitResult.GetActor());
		if (InteractingNPC != nullptr)
		{
			InteractingPersona = Cast<UPersonaComponent>(InteractingNPC->GetCharacterPersona());
			if (InteractingPersona != nullptr)
			{
				if (WidgetClass)
				{
					InteractionWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
					if (InteractionWidget)
					{
						InteractionWidget->AddToViewport();
						UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = true;

						// Bind the OnTextCommitted event
						UEditableText* InteractionText = Cast<UEditableText>(InteractionWidget->WidgetTree->FindWidget("InputText"));
						if (InteractionText)
						{
							InteractionText->OnTextCommitted.AddDynamic(this, &ANPC_Chatbot_DemoCharacter::HandleTextCommitted);
						}
					}
				}
			}
		}
	}
}

void ANPC_Chatbot_DemoCharacter::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString EnteredText = Text.ToString();
		UE_LOG(LogTemp, Display, TEXT("Text entered: %s"), *EnteredText);

		if (InteractingNPC && InteractingPersona)
		{
			InteractingPersona->GenerateResponse(EnteredText);
			InteractingPersona->OnMessageReceived.AddDynamic(this, &ANPC_Chatbot_DemoCharacter::OnMessageReceived);
		}
	}
	
}

void ANPC_Chatbot_DemoCharacter::OnMessageReceived(const FString& Message)
{
	if (InteractionWidget)
	{
		UMultiLineEditableTextBox* OutputText = Cast<UMultiLineEditableTextBox>(InteractionWidget->WidgetTree->FindWidget("OutputText"));
		if (OutputText)
		{
			OutputText->SetText(FText::FromString(Message));
		}
	}
}



