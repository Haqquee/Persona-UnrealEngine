import os
import json
from model import model
from langchain.chains import LLMChain
from langchain.memory.buffer import ConversationBufferMemory
from persona_memory import PersonaMemory
from prompt_template import ue_chatbot_prompt_tempalate
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

#Keep track of all the created personas
personas = {}

#Directory for persona memory
MEMORY_ARCHIVE = "persona_memory_archive"
os.makedirs(MEMORY_ARCHIVE, exist_ok=True)

def create_persona(name: str, background: str):
    memory = PersonaMemory(memory_key="conversation_history", return_messages=True)
    memory_archive = os.path.join(MEMORY_ARCHIVE, f"{name}_memory.json")

    if os.path.exists(memory_archive):
        memory.load_from_file(memory_archive)

   # memory =  ConversationBufferMemory(memory_key="conversation_history", return_messages=True)
    partial_prompt = ue_chatbot_prompt_tempalate.partial(persona_name=name, persona_background=background) 
    conversation_chain = LLMChain(llm=model,
                                           prompt=partial_prompt,
                                           memory=memory
                                           )
    return conversation_chain

#Define Pydantic models for API requests
class CreatePersonaRequest(BaseModel):
    name: str
    background: str

class ChatRequest(BaseModel):
    name: str
    input: str

class DeletePersonaRequest(BaseModel):
    name: str

class ClearMemoryRequest(BaseModel):
    name: str

#Initialize API
app = FastAPI()

@app.get("/")
async def root():
    return {"output": "App is running."}

@app.post("/create_persona")
async def create_persona_endpoint(creation_request: CreatePersonaRequest):
    name = creation_request.name
    background = creation_request.background
    memory_archive = os.path.join(MEMORY_ARCHIVE, f"{name}_memory.json")

    if name in personas or os.path.exists(memory_archive):
        if name not in personas:
            personas[name] = create_persona(name, background)
        return {"output": f"Persona with name '{name}' is already available."}
    
    personas[name] = create_persona(name, background)  
    return {"output": f"Hi my name is {name}."}

@app.post("/chat")
async def chat_endpoint(chat_request: ChatRequest):
    name = chat_request.name
    user_input = chat_request.input
    if name not in personas:
        memory_archive = os.path.join(MEMORY_ARCHIVE, f"{name}_memory.json")
        if os.path.exists(memory_archive):
            personas[name] = create_persona(name, "")
        else:
            raise HTTPException(status_code=404, detail=f"Persona with name '{name}' does not exist.")
    
    conversation_chain = personas[name]
    response = conversation_chain.invoke(user_input)

    #Write conversation to file
    memory_archive = os.path.join(MEMORY_ARCHIVE, f"{name}_memory.json")
    conversation_chain.memory.save_to_file(memory_archive)
    return {"output": response["text"]}

@app.post("/clear_history")
async def clear_memory_endpoint(request: ClearMemoryRequest):
    name = request.name
    if name not in personas:
        raise HTTPException(status_code=404, detail=f"Persona with name '{name}' does not exist.")
    memory = personas[name].memory
    memory.clear()  # Clear the conversation history

    memory_archive = os.path.join(MEMORY_ARCHIVE, f"{name}_memory.json")
    if os.path.exists(memory_archive):
        with open(memory_archive, 'w') as file:
            json.dump([], file)

    return {"output": f"Memory for {name} has been cleared."}

@app.delete("/delete_persona")
async def delete_persona_endpoint(request: DeletePersonaRequest):
    name = request.name
    if name not in personas:
        raise HTTPException(status_code=404, detail=f"Persona with name '{name}' does not exist.")
    del personas[name]  # Remove the persona

    memory_archive = os.path.join(MEMORY_ARCHIVE, f"{name}_memory.json")
    if os.path.exists(memory_archive):
        os.remove(memory_archive)

    return {"output": f"Persona {name} has been deleted."}