import paho.mqtt.client as mqtt
import json
import pymongo
import functions
from datetime import datetime

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("620155671")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    try:
        payload = msg.payload.decode("utf-8")

        payload_json = json.loads(payload)
        payload_json['timestamp'] = datetime.timestamp(datetime.now())
        functions.mycol.insert_one(payload_json)
        print(payload)
    except json.JSONDecodeError:
        print(f"Failed to decode JSON: {payload}")

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("broker.emqx.io", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
