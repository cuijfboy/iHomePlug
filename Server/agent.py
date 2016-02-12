import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=38400,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

from flask import Flask
app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello World, I am iHomeAgent ^_^!'

@app.route('/up/on')
def upPlugOn():
    ser.write('5505020000F040'.decode('hex'))
    return 'iHomeAgent : Plug : Up : ON !'

@app.route('/up/off')
def upPlugOff():
    ser.write('55050200000040'.decode('hex'))
    return 'iHomeAgent : Plug : Up : OFF !'

@app.route('/down/on')
def downPlugOn():
    ser.write('5505020000F020'.decode('hex'))
    return 'iHomeAgent : Plug : Down : ON !'

@app.route('/down/off')
def downPlugOff():
    ser.write('55050200000020'.decode('hex'))
    return 'iHomeAgent : Plug : Down : OFF !'

if __name__ == '__main__':
    ser.isOpen()
    app.debug = True
    app.run(host='0.0.0.0', port=80)
