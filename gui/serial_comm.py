import serial
import time

class SerialComm:
    def __init__(self, port='COM5', baudrate=115200):
        # Deschide conexiunea serială pe portul COM3 (sau alt port)
        self.ser = serial.Serial(port, baudrate, timeout=1)

    def send_data(self, data):
        # Trimite datele la microcontroller
        self.ser.write(data.encode())  # Codifică șirul de caractere într-un byte
        print(f"Sent: {data}")

    def read_data(self):
        # Citește datele primite de la microcontroller
        if self.ser.in_waiting > 0:
            data = self.ser.readline().decode().strip()
            print(f"Received: {data}")
            return data
        return None

    def close(self):
        # Închide conexiunea serială
        self.ser.close()

# Exemplar de utilizare
serial_comm = SerialComm('COM5', 115200)

# Trimite un mesaj către microcontroller
serial_comm.send_data("Hello from PC")

# Așteaptă să primească un mesaj de la microcontroller
while True:
    response = serial_comm.read_data()
    if response:
        print(f"Response: {response}")
    time.sleep(1)

# Închide conexiunea serială la final
serial_comm.close()
