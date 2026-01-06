from redis import Redis

from .config import Config
from .services.message import MessageService


def redis_listener():
    r = Redis(host=Config.REDIS_HOST, port=Config.REDIS_PORT)
    print("Starting Redis listener...")
    pubsub = r.pubsub()
    # subscribe to all the channels and set their callables
    pubsub.subscribe(
        **{
            "chuveiro:telemetria:notifications": MessageService.handle_telemetria,
            "chuveiro:status:notifications": MessageService.handle_status,
            # "chuveiro:limite:notifications": MessageService.handle_limite,
        }
    )
    for message in pubsub.listen():
        if message['type'] == 'message':
            print(f"Received message on channel {message['channel'].decode()}")
