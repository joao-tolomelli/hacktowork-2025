import json
import paho.mqtt.client as mqtt
import redis

from config import Config


# The callback for when the client receives a CONNACK response from the server.
def create_on_connect(topic_to_subscribe):
    def on_connect(client, userdata, flags, rc):
        """Handles the result of the connection attempt."""
        if rc == 0:
            print("Connected successfully in anonymous mode!")
            # Subscribing in on_connect() means that if we lose the connection and
            # reconnect, the subscriptions will be renewed automatically.
            print(f"Subscribing to topic: {topic_to_subscribe}")
            client.subscribe(topic_to_subscribe)
        else:
            print(f"Connection failed with code {rc}. Check broker status.")
    return on_connect

# The callback for when a PUBLISH message is received from the broker.
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
                data = msg.payload.decode()
                json_data = json.loads(data)
                sensor_id = json_data.get("sensor_id", "sensor")
                metrics = json_data.get("metrics", {})
                # r.lpush(f"mqtt_messages:{sensor_id}", json.dumps(metrics))
                # i want to store each new metric collection as an entry in a existing list
                r.rpush(f"mqtt_messages:{sensor_id}", json.dumps(metrics))
            except Exception as e:
                print(f"Failed to store message in Redis: {e}")
    return on_message

# The callback for when a SUBACK is received. (Optional, good for debugging)
def on_subscribe(client, userdata, mid, granted_qos):
    print(f"Subscription confirmed (QoS: {granted_qos})")


def main():
    BROKER_HOST = Config.BROKER_HOST
    BROKER_PORT = Config.BROKER_PORT
    TOPIC_TO_SUBSCRIBE = Config.TOPIC_TO_SUBSCRIBE
    # =================================================================
    # 2. Client Setup and Main Loop
    # =================================================================

    # Create the MQTT client instance
    client = mqtt.Client()
    on_connect = create_on_connect(TOPIC_TO_SUBSCRIBE)
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
    print("Client is listening indefinitely. Press Ctrl+C to stop.")
    client.loop_forever()


if __name__ == "__main__":
    main()
