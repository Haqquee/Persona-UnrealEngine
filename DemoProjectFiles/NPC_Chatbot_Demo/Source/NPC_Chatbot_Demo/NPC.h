// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC.generated.h"

UCLASS()
class NPC_CHATBOT_DEMO_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Persona, meta = (AllowPrivateAccess = "true"))
	class UPersonaComponent* CharacterPersona;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPersonaComponent* GetCharacterPersona() const { return CharacterPersona; }
	//USkeletalMeshComponent* GetCharacterMesh() const { return CharacterMesh; }
	

};
