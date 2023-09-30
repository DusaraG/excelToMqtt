import pandas as pd
import paho.mqtt.client as mqtt
import json
import time

# Define MQTT broker and topic information
broker_address = "broker.mqtt.cool"  # Replace with your MQTT broker address
topic = "Group1"  # Replace with the MQTT topic where you want to publish the JSON
excel_file = "task1.xlsx"  # Replace with the full path to your Excel file
# Read data from Excel file into a DataFrame using 'openpyxl' engine


# Convert the DataFrame to a dictionary with a specific format


# Define the MQTT client
client = mqtt.Client("ExcelToMQTT")  # Choose a unique client ID

# Connect to the MQTT broker
client.connect(broker_address)

# Publish the JSON data to the specified topic
try:
    while True:
        
        try:
            df = pd.read_excel(excel_file, engine='openpyxl')
       
            
            for index, row in df.iterrows():
                data = {
                    "Location": row["Location"],
                    "Temperature": int(row["Temperature"]),
                    "Humidity": int(row["Humidity"]),
                    "Light": int(row["Light"])}
                json_data = json.dumps(data)
                client.publish(topic, json_data)
                print(f"Data from '{excel_file}' has been published to MQTT topic '{topic}'.")
                time.sleep(5)

            # Convert the dictionary to a JSON object
            
        except PermissionError:
            print(f"Please close '{excel_file}' before running the script.")
            time.sleep(5)
except KeyboardInterrupt:
    pass

# Disconnect from the MQTT broker
client.disconnect()




