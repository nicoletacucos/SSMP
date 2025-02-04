from PySide6.QtWidgets import QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QTextEdit
from PySide6.QtGui import QIcon, QFont
import pyqtgraph as pg
import serial
import time
from PySide6.QtCore import QTimer


class MainWindow(QMainWindow):
    promotie: str = "2024-2025"
    team: list[str] = [
        "ZUGRAVU PAUL",
        "CUCOȘ NICOLETA",
    ]

    def __init__(self, port='COM10', baudrate=14400):
        super().__init__()
        try:
            self.ser = serial.Serial(port, baudrate, timeout=1)
        except serial.SerialException as e:
            print(f"Error initializing serial port: {e}")
            self.ser = None

        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))

        main_layout = QVBoxLayout()
        left_layout = QVBoxLayout()

        # Plot widget 1
        self.plot_widget1 = pg.PlotWidget()  
        self.plot_widget1.setTitle("X (g) Values")
        self.plot_widget1.setLabel('left', 'Acceleration', units='g')
        self.plot_widget1.setLabel('bottom', 'Data Point', units='')
        self.plot_widget1.addLegend()

        self.x_values = []  
        self.data_counter = 0 
        self.plot_data_line = self.plot_widget1.plot(self.x_values, pen=pg.mkPen(color=(255, 0, 0), width=2), name="X (g)")

        
        left_layout.addWidget(self.plot_widget1)

        right_layout = QVBoxLayout()

        #Info
        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member)
        team_box_layout.addWidget(second_member)
        team_box.setLayout(team_box_layout)

        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)
        control_panel_box_layout = QVBoxLayout()

        invert_button = QPushButton("Invert LED Sequence")
        invert_button.clicked.connect(self.invert_led_sequence)
        control_panel_box_layout.addWidget(invert_button)
        control_panel_box.setLayout(control_panel_box_layout)

        right_layout.addWidget(team_box)
        right_layout.addWidget(control_panel_box)
        right_layout.addWidget(control_panel_box)
        right_layout.addWidget(control_panel_box)

        top_layout = QHBoxLayout()
        top_layout.addLayout(left_layout, 3)  
        top_layout.addLayout(right_layout, 1)  

        main_layout.addLayout(top_layout)

        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        debug_box = QGroupBox("Debug")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit)
        debug_box.setLayout(debug_box_layout)

        main_layout.addWidget(debug_box)

        widget = QWidget()
        widget.setLayout(main_layout)
        self.setCentralWidget(widget)

        #Timer date seriale
        self.timer = QTimer()
        self.timer.timeout.connect(self.read_serial_data)
        self.timer.start(100)  

    def send_data(self, data):
        if self.ser:
            try:
                self.ser.write(data.encode()) 
                self.text_edit.append(f"Sent to FRDM: {data}")
            except serial.SerialException as e:
                self.text_edit.append(f"Eroare trimitere date: {str(e)}")
        else:
            self.text_edit.append("Serial connection not initialized.")
    
    def append_text_with_timestamp(self, text):
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        self.text_edit.append(f"[{timestamp}] {text}")

    def update_plot(self, new_x_value):
        self.x_values.append(new_x_value)
        self.data_counter += 1

        self.plot_data_line.setData(range(self.data_counter), self.x_values)

    def read_serial_data(self):
        if self.ser:
            try:
                response = self.ser.readline().decode('utf-8', errors='ignore').strip()
                if response:
                    if response.startswith("X (g):"):
                        try:
                            x_g_str = response.split("X (g):")[1].strip()
                            x_g = float(x_g_str)
                            self.update_plot(x_g)
                        except ValueError:
                            self.append_text_with_timestamp(f"Eroare la conversia '{x_g_str}' la float.")
                    elif "Raw X:" in response:
                        self.append_text_with_timestamp(f"Received: {response}")
                    elif "X Offset:" in response:
                        self.append_text_with_timestamp(f"Received: {response}")
                    elif "STATE_" in response:
                        self.append_text_with_timestamp(f"{response}")
                    elif "PIT" in response:
                        self.append_text_with_timestamp(f"{response}")
                    elif "Calibrating..." in response:
                        self.append_text_with_timestamp(f"{response}")
                    elif "Eroare" in response:
                        self.append_text_with_timestamp(f"{response}")
            except serial.SerialException as e:
                self.text_edit.append(f"Eroare citire date: {str(e)}")
        else:
            self.text_edit.append("Serial connection not initialized.")

    def invert_led_sequence(self, data):
        self.text_edit.append("Invert Sequence Button Pushed")
        self.send_data("i")
        # response = self.ser.readline().decode().strip()  # Citește și decodifică datele
        # if response=="BLUE\n":
        #     self.text_edit.append(f"Received: {response}")

    def closeEvent(self, event):
        if self.ser:
            self.ser.close()
        super().closeEvent(event)