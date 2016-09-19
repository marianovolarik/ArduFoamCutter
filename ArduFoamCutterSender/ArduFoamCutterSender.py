#!/usr/bin/env python
# -*- coding: utf-8 -*-

# PyQt4-4.11.4-gpl-Py3.4-Qt5.5.0-x64
import sys
from PyQt4 import QtGui, uic
import serial
from serial.tools import list_ports


GREEN = 'green'
YELLOW = 'yellow'
RED = 'red'

FILE_OPENED = 'File %s opened.'
FILE_ERROR = 'Error opening the file %s.'
SEND_ERROR = 'Error sending bytes.'

MESSAGES_LEVEL = {
    FILE_OPENED: GREEN,
    FILE_ERROR: RED,
    SEND_ERROR: RED}


class ArduFoamCutterSender():
    def __init__(self):
        self.serial_port = None

    def list_serial_ports(self):
        return list_ports.comports()

    def open_port(self, port):
        self.serial_port = serial.Serial(
            port=port,
            baudrate=115200,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE)

    def close_port(self):
        self.serial_port.close()

    def send_command(self, line):
        try:
            line += '\n'
            self.serial_port.write(line.encode('ascii'))
            print(line.strip())
            while self.serial_port.inWaiting() <= 0:
                pass

            while self.serial_port.inWaiting() > 0:
                self.serial_port.read(1)
        except:
            print("Error")

        return line


class Window(QtGui.QDialog):
    def __init__(self):
        super(Window, self).__init__()
        uic.loadUi('window_layout.ui', self)

        self.select_file_b.clicked.connect(self.select_file)
        self.refresh_com_ports_b.clicked.connect(self.refresh_com_ports)
        self.send_b.clicked.connect(self.send)
        self.open_port_b.clicked.connect(self.open_port)
        self.close_port_b.clicked.connect(self.close_port)

        self.x_axis_b.clicked.connect(lambda: self.move_axis('X', False))
        self.n_x_axis_b.clicked.connect(lambda: self.move_axis('X', True))
        self.y_axis_b.clicked.connect(lambda: self.move_axis('Y', False))
        self.n_y_axis_b.clicked.connect(lambda: self.move_axis('Y', True))
        self.a_axis_b.clicked.connect(lambda: self.move_axis('A', False))
        self.n_a_axis_b.clicked.connect(lambda: self.move_axis('A', True))
        self.z_axis_b.clicked.connect(lambda: self.move_axis('Z', False))
        self.n_z_axis_b.clicked.connect(lambda: self.move_axis('Z', True))

        self.cnc = ArduFoamCutterSender()
        self.filename = ''
        self.refresh_com_ports()

    def select_file(self):
        filename = QtGui.QFileDialog(self).getOpenFileName()
        if filename:
            self.filename = filename
            self.selected_file_l.setText(self.filename)
            self.show_status(FILE_OPENED % filename, MESSAGES_LEVEL[FILE_OPENED])

    def refresh_com_ports(self):
        self.com_ports_cb.clear()
        for port in self.cnc.list_serial_ports():
            self.com_ports_cb.addItem(port.device)

    def send(self):
        try:
            f = open(self.filename)
        except FileNotFoundError:
            self.show_status(FILE_ERROR % self.filename, MESSAGES_LEVEL[FILE_ERROR])
            return

        for line in f:
            if not self.cnc.send_command(line):
                self.show_status(SEND_ERROR, MESSAGES_LEVEL[SEND_ERROR])
                break

        f.close()

    def open_port(self):
        self.cnc.open_port(self.com_ports_cb.currentText())

    def close_port(self):
        self.cnc.close_port()

    def move_axis(self, axis, negative):
        self.cnc.send_command("G91")
        value = ('-' if negative else '') + str(self.step_sp.value())
        self.cnc.send_command("G0 %s%s" % (axis, value))

    def show_status(self, message, level):
        self.status_l.setText(message)
        self.status_l.setStyleSheet('color: %s' % (level))


if __name__ == '__main__':

    app = QtGui.QApplication(sys.argv)
    w = Window()
    w.show()
    sys.exit(app.exec_())
