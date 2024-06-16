#include "PersonaComponent.h"
#include "Json.h"

// Sets default values for this component's properties
UPersonaComponent::UPersonaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UPersonaComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set default values in case they are empty when the game starts
	if (Name.IsEmpty())
	{
		Name = "Hamie";
	}

	if (Background.IsEmpty())
	{
		Background = "You are a default Unreal Engine Bot powered by an LLM, and optimized by NVIDIA.";
	}

	if (CreatePersonaURL.IsEmpty())
	{
		CreatePersonaURL = "http://127.0.0.1:8000/create_persona";
	}

	if (ChatURL.IsEmpty())
	{
		ChatURL = "http://127.0.0.1:8000/chat";
	}

	InitializePersona();

}

// Called every frame
void UPersonaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPersonaComponent::InitializePersona()
{
	// Creates a new JSON object with the Persona name and background
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("name"), Name);
	JsonObject->SetStringField(TEXT("background"), Background);
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// Creates a new POST request for the API with the required information
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(CreatePersonaURL);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);

	// Binds this request to a function that handles the response from the API
	Request->OnProcessRequestComplete().BindUObject(this, &UPersonaComponent::OnInitializePersonaResponse);
	Request->ProcessRequest();
}

void UPersonaComponent::OnInitializePersonaResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully && Response.IsValid())
	{
		FString ResponseContent = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Initialized persona: %s"), *Name); // For debugging only (remove when needed)
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create persona:%s"), *Name); // For debugging only (remove when needed)
	}
}

// Sends a message to the API for the LLM to respond to
void UPersonaComponent::GenerateResponse(FString& UserInput)
{
	SendMessageRequest(Name, UserInput);
}

void UPersonaComponent::SendMessageRequest(FString& PersonaName, FString& Message)
{
	UE_LOG(LogTemp, Display, TEXT("Trying to send message request to the API")); // For debugging only (remove when needed)

	// Create JSON object with the message
	TSharedRef<FJsonObject> RequestObj = MakeShared<FJsonObject>();
	RequestObj->SetStringField("name", Name);
	RequestObj->SetStringField("input", Message);
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj, Writer);

	// Creates a new POST request for the API with the required information
	FHttpModule& Http = FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http.CreateRequest();
	Request->SetURL(ChatURL);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Accepts", "application/json");
	Request->SetContentAsString(RequestBody);
	Request->SetTimeout(120);

	// Binds this request to a function that handles the response from the API
	Request->OnProcessRequestComplete().BindUObject(this, &UPersonaComponent::OnResponseReceived);
	Request->ProcessRequest();
}

void UPersonaComponent::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully && Response.IsValid())
	{
		// Parse the JSON response
		FString ResponseContent = Response->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			// Extract the output string
			FString OutputString;
			if (JsonObject->TryGetStringField(TEXT("output"), OutputString))
			{
				UE_LOG(LogTemp, Log, TEXT("Output from LLM: %s"), *OutputString); // For debugging only (remove when needed)
				OnMessageReceived.Broadcast(OutputString);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Output field 'output' not found in JSON response.")); // For debugging only (remove when needed)
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON response.")); // For debugging only (remove when needed)
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to receive valid response.")); // For debugging only (remove when needed)
	}
}


