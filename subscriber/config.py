import os
from dotenv import load_dotenv

load_dotenv()


class Config:
    BROKER_HOST = os.environ.get("BROKER_HOST", "localhost")
    BROKER_PORT = int(os.environ.get("BROKER_PORT", 1883))

    USE_REDIS = os.environ.get("USE_REDIS", "1").lower() in ("true", "1", "t")
    REDIS_HOST = os.environ.get("REDIS_HOST", "localhost")
    REDIS_PORT = os.environ.get("REDIS_PORT", 6379)
