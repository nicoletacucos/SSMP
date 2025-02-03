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

    def __init__(self, port='COM5', baudrate=115200):
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
        plot_widget1 = pg.PlotWidget()
        plot_widget1.setTitle("Plot 1")
        plot_widget1.setLabel('left', 'Temperature', units='°C')
        plot_widget1.setLabel('bottom', 'Hour', units='h')

        hour = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        temperature = [30, 32, 34, 32, 33, 31, 29, 32, 35, 45]
        plot_widget1.plot(hour, temperature)

        # Plot widget 2
        plot_widget2 = pg.PlotWidget()
        plot_widget2.setTitle("Plot 2")
        plot_widget2.setLabel('left', 'Pressure', units='Pa')
        plot_widget2.setLabel('bottom', 'Hour', units='h')

        pressure = [1010, 1020, 1030, 1025, 1028, 1015, 1005, 1010, 1020, 1030]
        plot_widget2.plot(hour, pressure)

        left_layout.addWidget(plot_widget1)
        left_layout.addWidget(plot_widget2)

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

    def read_serial_data(self):
        if self.ser:
            try:
                response = self.ser.readline().decode().strip() #decodifica datele primite
                if response:
                    if "Invert Sequence Command\n" not in self.text_edit.toPlainText():
                        self.text_edit.append(f"Received: {response}")
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