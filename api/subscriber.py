import paho.mqtt.client as mqtt
import time

# Broker Details
BROKER_HOST = "localhost"
BROKER_PORT = 1883
TOPIC_TO_SUBSCRIBE = "test/anonymous_mqtt" # The same topic your publisher uses

# =================================================================
# 1. Callback Functions
# =================================================================

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    """Handles the result of the connection attempt."""
    if rc == 0:
        print("Connected successfully in anonymous mode!")
        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect, the subscriptions will be renewed automatically.
        print(f"Subscribing to topic: {TOPIC_TO_SUBSCRIBE}")
        client.subscribe(TOPIC_TO_SUBSCRIBE)
    else:
        print(f"Connection failed with code {rc}. Check broker status.")

# The callback for when a PUBLISH message is received from the broker.
def on_message(client, userdata, msg):
    """Handles incoming messages for subscribed topics."""
    print("--- Message Received ---")
    print(f"Topic: {msg.topic}")
    # The message payload is a byte string, so we decode it to UTF-8
    print(f"Payload: {msg.payload.decode()}")
    print(f"QoS: {msg.qos}")
    print("------------------------")

# The callback for when a SUBACK is received. (Optional, good for debugging)
def on_subscribe(client, userdata, mid, granted_qos):
    print(f"Subscription confirmed (QoS: {granted_qos})")


# =================================================================
# 2. Client Setup and Main Loop
# =================================================================

# Create the MQTT client instance
client = mqtt.Client()

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