from PySide6.QtWidgets import QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QTextEdit
from PySide6.QtGui import QIcon, QFont
import pyqtgraph as pg
import serial


class MainWindow(QMainWindow):
    promotie: str = "2024-2025"
    team: list[str] = [
        "ZUGRAVU PAUL",
        "CUCOȘ NICOLETA",
    ]

    def __init__(self):
        super().__init__()
        self.serial_port = serial.Serial('COM5', 115200, timeout=1)
        self.setWindowTitle(f"Proiect Microprocesoare {self.promotie}")
        self.setWindowIcon(QIcon("./icon.png"))

        # Primary layout - vertical layout for top part and debug at the bottom
        main_layout = QVBoxLayout()

        # Left part layout (for plots)
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

        # Add both plots to the left layout
        left_layout.addWidget(plot_widget1)
        left_layout.addWidget(plot_widget2)

        # Right part layout (for team info and control panel)
        right_layout = QVBoxLayout()

        # Team info box
        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.setSpacing(0)  # No spacing between team info widgets
        team_box_layout.addWidget(first_member)
        team_box_layout.addWidget(second_member)
        team_box.setLayout(team_box_layout)

        # Control panel box
        control_panel_box = QGroupBox("Control Panel")
        control_panel_box.setFont(bold_font)

        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(0)  # No spacing between control panel widgets

        # Button to invert LED sequence
        invert_button = QPushButton("Invert LED Sequence")
        invert_button.clicked.connect(self.invert_led_sequence)
        control_panel_box_layout.addWidget(invert_button)
        control_panel_box.setLayout(control_panel_box_layout)

       
        right_layout.addWidget(team_box)
        right_layout.addWidget(control_panel_box)
        right_layout.addWidget(control_panel_box)
        right_layout.addWidget(control_panel_box)


        # Add left and right layouts to the top of the main layout
        top_layout = QHBoxLayout()
        top_layout.addLayout(left_layout, 3)  # Set width ratio for left part (3)
        top_layout.addLayout(right_layout, 1)  # Set width ratio for right part (1)

        main_layout.addLayout(top_layout)

        # Debug box (optional, for logging messages)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        debug_box = QGroupBox("Debug")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit)
        debug_box.setLayout(debug_box_layout)

        # Add debug box to the bottom of the main layout
        main_layout.addWidget(debug_box)

        # Set the central widget
        widget = QWidget()
        widget.setLayout(main_layout)
        self.setCentralWidget(widget)

    def invert_led_sequence(self):
        try:
            self.serial_port.write(b'INVERT\n')  # Trimite comanda
            self.text_edit.insertPlainText("Comandă trimisă: Invert LED Sequence.\n")
        except serial.SerialException as e:
            self.text_edit.insertPlainText(f"Eroare trimitere comandă: {str(e)}\n")
