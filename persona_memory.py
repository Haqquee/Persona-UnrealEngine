from langchain.agents import tool
from langchain.memory import ConversationBufferMemory
from langchain.schema import HumanMessage, AIMessage, SystemMessage
import json

#Custom memory class that allows the agent to write conversations into a file, and read previous conversations into memory
class PersonaMemory(ConversationBufferMemory):
    def save_to_file(self, filename: str):
        with open(filename, 'w') as f:
            json.dump([self.message_to_dict(msg) for msg in self.chat_memory.messages], f)

    def load_from_file(self, filename: str):
        try:
            with open(filename, 'r') as f:
                self.chat_memory.messages = [self.dict_to_message(msg_dict) for msg_dict in json.load(f)]
        except FileNotFoundError:
            self.clear()

    def clear(self):
        self.chat_memory.messages = []

    def message_to_dict(self, message):
        if isinstance(message, HumanMessage):
            return {"type": "human", "content": message.content}
        elif isinstance(message, AIMessage):
            return {"type": "ai", "content": message.content}
        elif isinstance(message, SystemMessage):
            return {"type": "system", "content": message.content}
        else:
            raise ValueError(f"Unsupported message type: {type(message)}")

    def dict_to_message(self, message_dict):
        message_type = message_dict["type"]
        if message_type == "human":
            return HumanMessage(content=message_dict["content"])
        elif message_type == "ai":
            return AIMessage(content=message_dict["content"])
        elif message_type == "system":
            return SystemMessage(content=message_dict["content"])
        else:
            raise ValueError(f"Unsupported message type: {message_type}")
