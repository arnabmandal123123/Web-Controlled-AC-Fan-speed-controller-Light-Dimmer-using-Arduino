import serial
import time
from flask import Flask, render_template, jsonify

# --- CONFIGURATION ---
# IMPORTANT: Change this to your Arduino's serial port.
SERIAL_PORT = 'COM3' 
BAUD_RATE = 9600

# --- INITIALIZE SERIAL CONNECTION ---
try:
    arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) 
    print(f"✅ Successfully connected to Arduino on {SERIAL_PORT}")
except serial.SerialException as e:
    print(f"❌ FAILED to connect to Arduino on {SERIAL_PORT}")
    print(f"   Error: {e}")
    print("   Please check the port name and ensure no other program is using the port.")
    exit()

# --- INITIALIZE FLASK WEB SERVER ---
app = Flask(__name__)

# --- WEB PAGE ROUTE ---
@app.route('/')
def index():
    """Renders the main control page from the 'templates' folder."""
    return render_template('index.html')

# --- API ROUTES FOR CONTROL ---
@app.route('/control/<mode>')
def control(mode):
    """Receives a command from the webpage and sends it to the Arduino."""
    command_to_send = ""
    if mode == 'on':
        command_to_send = "on\n"
    elif mode == 'half':
        command_to_send = "half\n"
    elif mode == 'off':
        command_to_send = "off\n"
    else:
        return jsonify(status="error", message="Invalid command"), 400

    try:
        arduino.write(command_to_send.encode())
        print(f"Sent command to Arduino: {command_to_send.strip()}")
        return jsonify(status="success", mode=mode)
    except Exception as e:
        print(f"Error writing to serial port: {e}")
        return jsonify(status="error", message="Failed to send command."), 500

# --- RUN THE SERVER ---
if __name__ == '__main__':
    print("--------------------------------------------------------------------")
    print("🚀 Starting the web server at http://127.0.0.1:5000")
    print("   Open your web browser to that address to control the Arduino.")
    print("   Press CTRL+C to quit.")
    print("--------------------------------------------------------------------")
    app.run(host='127.0.0.1', port=5000)

