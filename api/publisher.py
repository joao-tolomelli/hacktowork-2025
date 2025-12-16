import paho.mqtt.client as mqtt
import sys
import time

# Broker Details
BROKER_HOST = "localhost"
BROKER_PORT = 1883
TOPIC = "test/anonymous_mqtt"

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected successfully in anonymous mode!")
    else:
        print(f"Connection failed with code {rc}")

client = mqtt.Client()
client.on_connect = on_connect

# *** REMOVED: client.username_pw_set(USERNAME, PASSWORD) ***

# Connect to the broker
client.connect(BROKER_HOST, BROKER_PORT, 60)

# Start the loop in a non-blocking way
client.loop_start()

# Wait for connection to establish
time.sleep(1)

# Publish a message
try:
    message = sys.argv[1] if len(sys.argv) > 1 else "Hello from anonymous MQTT client!"
    print(f"Publishing message to topic: {TOPIC}")
    client.publish(TOPIC, message, qos=1)
except Exception as e:
    print(f"An error occurred during publishing: {e}")

# Wait to ensure message is sent
time.sleep(2)

client.loop_stop()
client.disconnect()
print("Disconnected.")
