# ESPTurismo

Software for capturing telemetry from Gran Turismo 7 on an ESP32, and controlling rumble motors for pedals.

Hardware:
    DoIT ESP32 devkit v1 (or other esp32 will do fine as well, configure the pins accordingly)
    A dual H-bridge motor controller (e.g mini L298N driver, https://www.aliexpress.com/item/33012645746.html)
    2x rumble motors, 5-6V (e.g https://www.aliexpress.com/item/4001082464065.html)

Connect VCC and GND from ESP32 to L298 driver, motors should run fine on 5V
Connect motor driver inputs to port 25,26,32 and 33

Build and upload filesystem:
platformIO->esp32doit->Platform->Build Filesystem Image
platformIO->esp32doit->Platform->Upload Filesystem Image
platformIO->Build
platformIO->Upload

Connect to usb power. After one minute ESP should go into access point mode if it can't connect to WiFi.
Connect with phone to http://10.0.0.1 and configure your wifi settins along with PS4/PS5 IP address.
Click apply, save and then reboot.

Alternatively, update the settings.json in the data folder with your wifi settings.

You should now be able to connect to http://ESPturismo.local after a short while. If GT7 is running telemitry
data should show up, and you can configure your rumble motor settings.

Sources for getting data from Gran Turismo:
    https://github.com/Bornhall/gt7telemetry/blob/main/gt7telemetry.py
    https://github.com/Nenkai/PDTools/blob/master/PDTools.SimulatorInterface/SimulatorPacket.cs

![screenshot](docs/screenshot.png)

One issue one might have, is electrical noise from the motors disturbing the esp32 or it's wifi reception.
I'm waiting for some capacitors for soldering between the motor connectors and the casing to reduce this,
and maybe also adding a choke between the ESP32 VCC/GND rail and the motor driver.