import json, time, threading, sys
from paho.mqtt import client as mqtt

BROKER = "IP add"
TOPIC  = "parking/spot/#"

state = {}      # spot_id -> dict
last_seen = {}  # spot_id -> timestamp
log = []        # reservation history

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
pause_refresh = False
OFFLINE_TIMEOUT = 10  # seconds before offline

# ANSI colors for readability
RED = "\033[91m"
GREEN = "\033[92m"
YELLOW = "\033[93m"
BLUE = "\033[94m"
RESET = "\033[0m"

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        spot = int(payload.get("spot", -1))
        state[spot] = payload
        last_seen[spot] = time.time()
        if not pause_refresh:
            print_table()
    except Exception as e:
        print("Error:", e)

def print_table():
    print("\033[2J\033[H", end="")  # clear screen
    print("Smart Parking Status\n")
    print("{:<8} {:<20} {:<12} {:<15} {:<10}".format(
        "Spot", "Status", "Distance", "IP", "Conn"
    ))
    print("-"*80)
    now = time.time()
    for k in sorted(state.keys()):
        p = state[k]
        seen = last_seen.get(k, 0)
        online = "ONLINE" if (now - seen) < OFFLINE_TIMEOUT else "OFFLINE"

        status = str(p.get("status",""))
        if "occupied" in status:
            status_col = RED + status + RESET
        elif "reserved" in status:
            status_col = YELLOW + status + RESET
        elif "free" in status:
            status_col = GREEN + status + RESET
        else:
            status_col = status

        print("{:<8} {:<20} {:<12} {:<15} {:<10}".format(
            p.get("spot",""),
            status_col,
            p.get("distance_cm",""),
            p.get("ip",""),
            online
        ))

    print("\nPress 'q' to reserve/unreserve, 'l' for logs, Ctrl+C to quit.")

def print_logs():
    print("\n--- Reservation Log ---")
    for entry in log[-10:]:
        print(entry)
    print("-----------------------\n")

def reserve_prompt():
    global pause_refresh
    pause_refresh = True
    try:
        spot = input("Enter spot number to toggle reservation (or Enter to cancel): ").strip()
        if spot.isdigit():
            spot = int(spot)
            if spot in state and "reserved" in str(state[spot].get("status")):
                client.publish(f"parking/control/{spot}", "clear")
                msg = f"[{time.strftime('%H:%M:%S')}] Spot {spot} unreserved"
                log.append(msg)
                print(msg)
            else:
                client.publish(f"parking/control/{spot}", "reserved")
                msg = f"[{time.strftime('%H:%M:%S')}] Spot {spot} reserved"
                log.append(msg)
                print(msg)
        else:
            print("No reservation made.")
        time.sleep(1)
    finally:
        pause_refresh = False
        print_table()

def input_thread():
    while True:
        cmd = input().strip().lower()
        if cmd == "q":
            reserve_prompt()
        elif cmd == "l":
            print_logs()

def main():
    client.on_message = on_message
    client.connect(BROKER, 1883, 60)
    client.subscribe(TOPIC)
    client.loop_start()

    threading.Thread(target=input_thread, daemon=True).start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        client.loop_stop()
        client.disconnect()
        sys.exit(0)

if __name__ == "__main__":
    main()

