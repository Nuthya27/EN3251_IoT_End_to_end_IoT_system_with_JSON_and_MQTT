import paho.mqtt.client as mqtt
import time
import json
import pandas as pd


# Callback when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker\n")
    else:
        print("Connection failed with code {rc}")


# Create an MQTT client instance
client = mqtt.Client("PythonPub")

# Set the callback function
client.on_connect = on_connect

broker_address = "test.mosquitto.org"  # broker's address
broker_port = 1883
keepalive = 5
qos = 0
#publish_topic = "Enigma-HomeConditions"

# Connect to the MQTT broker
client.connect(broker_address, broker_port, keepalive)

# Start the MQTT loop to handle network traffic
client.loop_start()

# Publish loop

try:
    while True:
        # Publish a message to the send topic
        df = pd.read_excel('data.xlsx')
        json_data = df.to_dict(orient='records')
        for i,record in enumerate(json_data):
            pub_topic = "Enigma-" + record["Location"]
            del record["Location"]
            data_out = json.dumps(record)
            client.publish(pub_topic,data_out)        
            print(f"Published message '{data_out}' to topic '{pub_topic}'\n")
        
        # Wait for a moment to simulate some client activity
        time.sleep(6)

except KeyboardInterrupt:
    # Disconnect from the MQTT broker
    pass
client.loop_stop()
client.disconnect()

print("Disconnected from the MQTT broker")
