# NodeMCU_to_azure
This programe is based on microsoft doc
https://docs.microsoft.com/en-us/samples/azure-samples/iot-hub-sparkfun-thingdev-client-app/spark-fun-esp8266-client/

install CH340 driver for

1. Dowload latest version of Arduino IDE (my version is 1.8.13, non-install version)

2. setup the nodemcu compatible to be allow to compile by arduino idle. Below is the link to do this
https://oranwind.org/-esp8266-nodemcu-zai-arduino-ide-she-ding-nodemcu/

copy the below link in the Additional boards Manager

http://arduino.esp8266.com/stable/package_esp8266com_index.json

If error occurs:

Skipping contributed index file C:\Users\xxxxx\AppData\Local\Arduino15\package_esp8266com_index.json, parsing error occured:
com.fasterxml.jackson.core.JsonParseException: Unexpected character ('<' (code 60)): expected a valid value (number, String, array, object, 'true', 'false' or 'null')
 at [Source: (FileInputStream); line: 1, column: 2]


based on the issues on github https://github.com/esp8266/Arduino/issues/6068
use the below link in preference
https://arduino.esp8266.com/stable/package_esp8266com_index.json

During its download, it comes to 4 parts. It stops in each part. So you have to click "install" again 4 times

3.Install the following libraries from Sketch -> Include library -> Manage libraries
Remember to make sure the publisher is by Microsoft

AzureIoTHub (mine is version 1.3.8)
AzureIoTUtility (mine is version 1.3.8)
AzureIoTProtocol_MQTT (mine is version 1.3.8)
ArduinoJson (mine is version 5.1.3) Don't use version 6.x.x some function is not available in this version
Adafruit_MCP3008 (if you are using MCP3008)

compile until all required library is installed


4. Then there will be error like this below when you compile

C:\Users\leolau\Documents\Arduino\libraries\AzureIoTHub\src\iothub_client_ll_uploadtoblob.c:26:27: fatal error: internal/blob.h: No such file or directory
 #include "internal/blob.h"
                           ^
compilation terminated.
exit status 1
Error compiling for board NodeMCU 1.0 (ESP-12E Module).

Here is the most on9 part. You have to change some words inside azure library in order to make it works.
below are the links to the official Azure+Arduino library:
https://github.com/Azure/azure-iot-arduino

below are the instruction
Navigate to  C:\Users\<your user name>\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.7.1\cores\esp8266

Locate the board's Arduino.h (hardware/esp8266/<board package version>/cores/esp8266/ and comment out the line containing #define round(x), around line 137.

Two folders up from the Arduino.h step above, in the same folder as the board's platform.txt, paste the platform.local.txt file from the esp8266 folder in the sample into it.
(directories:  C:\Users\leolau\Documents\Arduino\libraries\AzureIoTHub\examples\iothub_ll_telemetry_sample\esp8266)

Note1: It is necessary to add -DDONT_USE_UPLOADTOBLOB and -DUSE_BALTIMORE_CERT to build.extra_flags= in a platform.txt in order to run the sample, however, you can define them in your own platform.txt or a platform.local.txt of your own creation.

Note2: If your device is not intended to connect to the global portal.azure.com, please change the CERT define to the appropriate cert define as laid out in certs.c

Note3: Due to RAM limits, you must select just one CERT define.

5. Next Error : 

exec: "{runtime.tools.python.path}/python": file does not exist
Error compiling for board NodeMCU 1.0 (ESP-12E Module).

There is also an issues related to this issues in the github
https://github.com/esp8266/Arduino/issues/5881
But I don't use what he suggest. Rather, I follow the arduino master to do this:
https://forum.arduino.cc/index.php?topic=663500.0

Do this:
Open the platform.local.txt file you saved to the ESP8266 core in a text editor.

Delete the contents of the file.

Add this single line of code:
Code: [Select]
build.extra_flags=-DESP8266 -DDONT_USE_UPLOADTOBLOB -DUSE_BALTIMORE_CERT

viola!
Done compile

P:\Nodemcu\NodeMcu\Azure_NodeMCU+MCP3008 +GPS\app\app.ino:61:33: warning: deprecated conversion from string constant to 'char*' [-Wwrite-strings]
 static char *connectionString = "HostName=gammon-iot-dev.azure-devices.net;DeviceId=esp8266_leo;SharedAccessKey=jh1uXXyUY0q2lhlMpnJFVmNLMqo0htVZMLH0EjPFF9U=";
