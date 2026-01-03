import paho.mqtt.client as mqtt
import redis
import threading

from config import Config


# The callback for when the client receives a CONNACK response from the server.
def create_on_connect(topics_to_subscribe: list[str]):
    def on_connect(client, userdata, flags, rc):
        """Handles the result of the connection attempt."""
        if rc == 0:
            print("Connected successfully in anonymous mode!")
            # Subscribing in on_connect() means that if we lose the connection and
            # reconnect, the subscriptions will be renewed automatically.
            client.subscribe([(topic, 1) for topic in topics_to_subscribe])
            print(f"Subscribed to topics: {topics_to_subscribe}")
        else:
            print(f"Connection failed with code {rc}. Check broker status.")
    return on_connect

def create_on_message(use_redis, redis_host, redis_port):
    r = redis.Redis(host=redis_host, port=redis_port) if use_redis else None
    def on_message(client, userdata, msg):
        """Handles incoming messages for subscribed topics."""
        print("--- Message Received ---")
        print(f"Topic: {msg.topic}")
        # The message payload is a byte string, so we decode it to UTF-8
        print(f"Payload: {msg.payload.decode()}")
        print(f"QoS: {msg.qos}")
        print("------------------------")
        if r:
            try:
                match msg.topic:
                    case "chuveiro/telemetria":
                        redis_key = "chuveiro:telemetria"
                    case "chuveiro/status":
                        redis_key = "chuveiro:status"
                    case "chuveiro/limite":
                        print("LIMITE message received")
                        return  # o propósito de estar inscrito neste tópico é apenas monitorar o limite.
                    case _:
                        redis_key = "chuveiro:unknown"
                r.rpush(redis_key, msg.payload.decode())
                r.publish(f"{redis_key}:notifications", msg.payload.decode())
                print(f"Stored message in Redis list {redis_key}")
            except Exception as e:
                print(f"Failed to store message in Redis: {e}")
    return on_message

# The callback for when a SUBACK is received. (Optional, good for debugging)
def on_subscribe(client, userdata, mid, granted_qos):
    print(f"Subscription confirmed (QoS: {granted_qos})")


def redis_listener(mqtt_client):
    r = redis.Redis(host=Config.REDIS_HOST, port=Config.REDIS_PORT)
    pubsub = r.pubsub()
    pubsub.subscribe("chuveiro:limite:notifications")
    print("Redis listener started, waiting for limite notifications...")
    for message in pubsub.listen():
        if message['type'] == 'message':
            print(f"Redis Notification - New limite value: {message['data'].decode()}")
            mqtt_client.publish("chuveiro/limite", "ESTOURO", qos=1)


def main():
    BROKER_HOST = Config.BROKER_HOST
    BROKER_PORT = Config.BROKER_PORT
    TOPICS_TO_SUBSCRIBE = ["chuveiro/telemetria", "chuveiro/status", "chuveiro/limite"]
    # =================================================================
    # 2. Client Setup and Main Loop
    # =================================================================

    # Create the MQTT client instance
    client = mqtt.Client()
    on_connect = create_on_connect(TOPICS_TO_SUBSCRIBE)
    on_message = create_on_message(Config.USE_REDIS, Config.REDIS_HOST, Config.REDIS_PORT)

    # Assign the callback functions
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_subscribe = on_subscribe

    # Connect to the broker
    print(f"Attempting to connect to {BROKER_HOST}:{BROKER_PORT}...")
    try:
        client.connect(BROKER_HOST, BROKER_PORT, 60)
    except Exception as e:
        print(f"Connection error: {e}")
        exit()

    # Blocking call that processes network traffic, dispatches callbacks,
    # and handles reconnecting. It blocks the thread.

    print("Starting Redis listener thread...")
    threading.Thread(target=redis_listener, args=(client,), daemon=True).start()
    print("Redis listener thread started.")
    print("Client is listening indefinitely. Press Ctrl+C to stop.")
    client.loop_forever()


if __name__ == "__main__":
    main()
