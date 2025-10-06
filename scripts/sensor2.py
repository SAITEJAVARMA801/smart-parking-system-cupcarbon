import paho.mqtt.client as mqtt
import json
import time

def cup(com):
    print(com, flush=True)

BROKER = "ip add"
TOPIC  = "parking/spot/2"   
SPOT_ID = 2

reserved = False
occupied = False
distance = 0.0
last_state = None
dirty = False

def update_display():
    global last_state, dirty
    state = (reserved, occupied, distance)
    if state == last_state and not dirty:
        return
    last_state = state
    dirty = False

    # Status decision
    if occupied and reserved:
        status = "RES+OCC"
        cup("mark")  # LED ON
        cup(f"alert Spot{SPOT_ID}: {status} d≈{distance:.1f} cm")
    elif occupied:
        status = "OCCUPIED"
        cup("mark")
        cup(f"alert Spot{SPOT_ID}: {status} d≈{distance:.1f} cm")
    elif reserved and not occupied:
        status = "RESERVED FREE"
        cup("unmark")
        cup(f"print Spot{SPOT_ID}: {status} d≈{distance:.1f} cm")
    else:
        status = "FREE"
        cup("unmark")
        cup(f"message Spot{SPOT_ID}: {status} d≈{distance:.1f} cm")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        cup("message MQTT Connected")
        client.subscribe(TOPIC)
    else:
        cup(f"alert MQTT Fail {rc}")

def on_message(client, userdata, msg):
    global occupied, distance, dirty
    try:
        payload = json.loads(msg.payload.decode())
        distance = float(payload.get("distance_cm", 0))
        status = str(payload.get("status", "")).lower()
        occupied = "occupied" in status
        dirty = True
    except Exception as e:
        cup(f"print Bad payload: {e}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, 1883, 60)
client.loop_start()

while True:
    # Node click toggles reserved
    cup("getaction")
    act = input().strip()
    if act == "1":
        reserved = not reserved
        dirty = True
    update_display()
    time.sleep(0.05)
