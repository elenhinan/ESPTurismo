# ESPTurismo

Software for capturing telemetry from Gran Turismo 7 on an ESP32, and controlling rumble motors for pedals.

## Features:

### Rumble can be controlled by:
* Wheel slip (maximum or average of all four wheels)
* Rev limit as reported by GT7
* TCS as reported by GT7
* ASM as reported by GT7

### Rumble will not be enabled:
* If not in game
* If speed below set limit
* If pedal input below set limit

Output level can be set per pedal, to a constant level (so no dynamic rumbling as of yet).

## Hardware:
### Parts needed
* Wemos S2 mini / DoIT ESP32 devkit v1
* A dual H-bridge motor controller (e.g mini L298N driver, https://www.aliexpress.com/item/33012645746.html)
* 2x rumble motors, 5-6V (e.g https://www.aliexpress.com/item/4001082464065.html)

### Connections
Connect VCC and GND from ESP32 to L298 driver, motors should run fine on 5V. Maybe add a choke on the power lines
Connect motor driver inputs to port 2,4,6,8 for s2 mini or 25,26,32,33 for ESP32 devkit
Twist the motor wire-pair to reduce EMI, add ceramic capacitors (I use 68 nF) from motor inputs to chassis (so four in total)

### Build and upload filesystem:
Use Visual Studio Code with PlatformIO:
* edit settings.json with your WiFi SSID and password (optional)
* platformIO->esp32doit->Platform->Build Filesystem Image
* platformIO->esp32doit->Platform->Upload Filesystem Image
* platformIO->Build
* platformIO->Upload

### Configuring
Connect to usb power. After one minute ESP should go into access point mode if it can't connect to WiFi. In that case,
connect phone to ESPturismo wifi AP. Go to http://10.0.0.1 in your browser and configure your wifi settings (if needed)
along with PS4/PS5 IP address. Click apply, save and then reboot.

You should now be able to connect to http://ESPturismo.local after a short while. If GT7 is running telemetry
data should show up, and you can configure your rumble motor settings. Click apply to test settings, and save if you're
satisfied.

## Sources
* https://github.com/Bornhall/gt7telemetry/blob/main/gt7telemetry.py
* https://github.com/Nenkai/PDTools/blob/master/PDTools.SimulatorInterface/SimulatorPacket.cs

![screenshot](docs/screenshot.png)

If you have problems with a cheap wemos s2 clone rebooting, google "s2 mini capacitor fix" to see which capacitor needs
soldering. This should fix the instability. Buy an original to avoid the hassle.