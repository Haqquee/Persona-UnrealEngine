# Persona-UnrealEngine

## Introduction
**Persona** is a project developed with the aim to integrate Large-Language-Models and Unreal Engine, to power dynamic and immersive dialogues for characters in video games, without the need to create complex dialogue trees. At its core, Persona is an Unreal Engine component that can be attached to any actor within the game world. The component can then communicate with an LLM that is optimized and accelerated by NVIDIA and customized using the LangChain Framework. 

## How it Works
### NVIDIA and LangChain
This project is divided into two parts. The first is a generative AI application that uses LangChain and Mixtral-8x22b-Instruct-v0.1 LLM, hosted on NVIDIA NIM, to allow for the creation of Personas that can be chatted with seamlessly. This application takes the poweful tools provided by LangChain to setup conversation chains that helps structure the conversation based on the given input. A custom prompt template is defined to structure the input for the language model. Additionally, a custom memory class is extendended from LangChain's existing 'ConversationBufferMemory' class, to provide memory storage, where memory can be written to and loaded from files, allowing for consistent interactions across sessions. This application can be interacted with through a local API built using FastAPI.

### Unreal Engine
The second part of this project deals with Unreal Engine. A custom component called Persona Component is created using C++ within the engine. This component can be attached to any character or actor within the game-world (in Unreal Engine an actor is any object that can be placed into a level). The Persona component allows the user to give a character their own unique background, and the component contains necessary functions that allow for the character to interact with the game world with the use of LLM generated responses. It does this by communicating with the API that was created in the first part. The help of LangChain allows for these interactions to be stored in a memory archive for each individual character, so that they remember previous conversations and any useful data.

### Key Directories and Files

- **root/**: Project root
  - `app.py`: The main FastAPI application file.
  - `model.py`: Initializes the LLM used for the application (The API key is loaded here)
  - `persona_memory.py`: Custom memory class used for this project
  - `prompt_template.py`: Customizable prompt template to structure LLM's responses

- **PersonaComponent_UE/**: Contains the standalone Persona Component (Can be included in your own UE project)
  - `PersonaComponent.h`: Header file for the Persona Component in Unreal Engine.
  - `PersonaComponent.cpp`: Source file for the Persona Component in Unreal Engine.

- **DemoProjectFiles/**: Contains the Demo/Sample project files for Unreal Engine
  - `PersonaDemo.uproject`: Unreal Engine project file for the demo (This can be opened using Unreal Engine to run the demo).
  - `Content/`: Directory for Unreal Engine content assets.
  - `Config/`: Configuration files for the Unreal Engine project.
  - `Source/`: Source code for the Unreal Engine project.

## Installation (Windows only)
This repository contains:
- All the necessary files to run the local API for LLM inference.
- Persona component header and .cpp files for Unreal Engine (created and tested in UE 5.4).
- A demo Unreal Engine project that demonstrates the Persona Component and its current capabilities right out of the box.



### 1. Clone the Repository

```
git clone https://github.com/Haqquee/Persona-UnrealEngine
```

### 2. Install the Dependencies:
It is recommended to create a fresh virtual environment in Python to isolate the packages and their versions used for this project

```
pip install -r requirements.txt
```

### 3. Setup Environment Variables for LLM:
Create a .env file in the project root and add in NVIDIA NIM API key, or directly add the key in ```model.py```.

```
NVIDIA_API_KEY=your_nvidia_api_key
```

***NOTE:***
NVIDIA NIM is a part of NVIDIA AI Enterprise. If you do not have access to the API, it is possible to integrate a different LLM service (such as OpenAI) for this project, however, there is no guarantee that the performance of other services will be suitable. Take a look at the [LLMs supported by LangChain](https://python.langchain.com/v0.1/docs/integrations/llms/). Once you have a suitable LLM and inference service, it can be initialized in the ```model.py``` file in the repository, by replacing the current ```ChatNVIDIA``` model.

### 4. Run the API on http://127.0.0.1:8000/:

```
fastapi run main.py
```

Once the API is up and running, it is possible to start interacting with it outisde of Unreal Engine by sending HTTP requests.

<details>
<summary>API Endpoints:</summary>

- Create a Persona:
Create a new Persona by sending POST request to /create_persona with the persona's name and background.

```
POST /create_persona
{
  "name": "Mervin",
  "background": "Astronaut exploring the unknown."
}
```

- Chat with Persona:
Chat with a Persona by sending POST request to /chat.

```
POST /chat
{
  "name": "Mervin",
  "input": "Tell me about your last mission."
}
```

- Clear Persona's Memory:
Clear a Persona's memory by sending POST request to /clear_history.

```
POST /clear_history
{
  "name": "Mervin"
}
```

- Delete a Persona:
Delete a Persona by sending DELETE request to /delete_persona

```
DELETE /delete_persona
{
  "name": "Mervin"
}
```
</details>

### 5. Connecting to Unreal Engine:
If you do not have development experience with Unreal Engine or you want to just test the component, it is recommended to move on to the [Demo Project Section](#7-demo-project) which provides steps to use the demo project. Otherwise, here are the  steps to set it up for your own project:

<details>
<summary>Initialize the Persona Component for your own UE Project:</summary>

1. Copy the Persona Component's header and .cpp files into your Unreal Engine project code files and/or include them in your code. Your project must have C++ enabled.

Important Note:
It is important to include both the ```Http``` and the ```Json``` modules within your project's ```Build.cs``` file for proper communication with the API. Sample ```Build.cs``` file:

```
using UnrealBuildTool;

public class NPC_Chatbot_Demo : ModuleRules
{
	public NPC_Chatbot_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "HTTP", "Json"});
	}
}
```

2. Attach the Persona Component to the desired actor or character within the Unreal Engine editor. This can be done through either Blueprints or C++.
3. Modify the component's properties to set the character's name and background. Again, these properties are exposed to Blueprints, meaning that can be modified through Blueprints or directly through C++.

</details>

<details>
<summary>Using the Persona Component:</summary>

1. **Generate Response Function:**
The Persona Component uses the function ```GenerateResponse(FString& UserInput)``` to take any input and sends an HTTP request to the API. The response generated by the LLM is then sent back to the Engine, which processes the request and outputs it as string.  The function then uses a delegate to broadcast the received message.

2. **Setting up the Delegate:**
The delegate needs to be binded to a function in a class that will utilize the ```GenerateResponse``` function. For example, if you have a UI, and want the text generated by the LLM to be displayed into that UI (similar to what the demo project does), you can do the following:

```
void UExampleUIClass::SetupPersonaComponent(UPersonaComponent* PersonaComponent)
{
    if (PersonaComponent)
    {
        PersonaComponent->GenerateResponse("Sample Request");
        PersonaComponent->OnMessageReceived.AddDynamic(this, &UExampleUIClass::HandleMessage);
    }
}

void UExampleUIClass::HandleMessage(const FString& Message)
{
    // Update your UI with the message
    YourTextBlock->SetText(FText::FromString(Message));
}

```

</details>

### 6. Demo Project
A demo Unreal Engine project is included in this repository to give an example on how the Persona Component can be used. It sets up blueprints for the  Player, NPCs, UI along with the necessary functions for interactions between the Player and the Persona.
1. If you do not have Unreal Engine 5.4, it can be downloaded for free through the [Epic Games Launcher](https://www.unrealengine.com/en-US/download). To use Unreal Engine there are Hardware and Software requirements and prerequisites, see the [Hardware and Software Specifications for UE](https://dev.epicgames.com/documentation/en-us/unreal-engine/hardware-and-software-specifications-for-unreal-engine).
2. Launch Unreal Engine, which will open the project browser. On the bottom right, click the 'browse' button, to find and open the Unreal Engine Project file ```NPC_Chatbot_Demo``` included in this repository.
3. Given you have all the prerequisites mentioned in step 1, the project should start to compile (it may take a couple of minutes depending on hardware). If prompted, accept the rebuild option.
4. Once the project is opened, you can play the level in the editor.
- Use ```W```, ```A```, ```S```, ```D``` to move the player, and ```Space``` to jump.
- Hold ```E``` to interact with an NPC that has a Persona.
- Type text into the chatbox and press enter to chat with the Character.

## Generative AI Agents Developer Contest by NVIDIA and LangChain
This project was initially developed for the [Generative AI Agents Developer Contest by NVIDIA and LangChain](https://www.nvidia.com/en-us/ai-data-science/generative-ai/developer-contest-with-langchain/). It was a fun project to put together for the contest, however there are limitations and many improvements are to be made in the future.

## Current Limitations and Future Improvements
- During the time I had to do this project, I tried my best to make the setup as simple as I could, however, it still takes more steps than I would like for it to. It may be possible to simplify the steps and write a more cohesive documentation on how to use the Project with Unreal Engine. This will also give beginners of Unreal Engine and game devlopment, a better idea on how to use this component.
- The chat system used for this project, can be updated using [NVIDIA's NeMo Guardrails](https://github.com/NVIDIA/NeMo-Guardrails/tree/develop), to further control the output of the LLM for the characters in game.