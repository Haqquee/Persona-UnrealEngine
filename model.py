from dotenv import load_dotenv
import os
from langchain_nvidia_ai_endpoints import ChatNVIDIA

load_dotenv()

nvidia_api_key = os.getenv("NVIDIA_API_KEY")

#Initialize LLM using the ChatNVIDIA class in Langchain
#See the list of available models within the NVIDIA API Catalog: https://build.nvidia.com/explore/discover
model = ChatNVIDIA(model="mistralai/mixtral-8x22b-instruct-v0.1", 
                   api_key=nvidia_api_key)