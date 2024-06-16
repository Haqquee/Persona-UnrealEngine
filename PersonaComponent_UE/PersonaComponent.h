#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "PersonaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FString&, Message);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NPC_CHATBOT_DEMO_API UPersonaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPersonaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
	FString Background;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
	FString OutputResponse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
	FString CreatePersonaURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTTP")
	FString ChatURL;

	UFUNCTION(BlueprintCallable, Category = "Persona")
	void GenerateResponse(FString& UserInput);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnMessageReceived OnMessageReceived;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// HTTP Requests
	UFUNCTION(BlueprintCallable)
	void InitializePersona();

	void OnInitializePersonaResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	UFUNCTION(BlueprintCallable)
	void SendMessageRequest(FString& PersonaName, FString& Message);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



		
};
