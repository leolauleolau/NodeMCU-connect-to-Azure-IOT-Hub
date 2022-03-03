# Using NodeMCU(ESP8266) to send telementry to Azure IOT Hub
## Overview  
This repo is built because using the official code and document to connect ESP8266 to Azure IOT Hub is not working using the following source:  
ESP8266 Arduino SDK(version 2.7.1)  
https://github.com/esp8266/Arduino  
Microsoft document of ESP8266 to Azure IOT Hub   
https://docs.microsoft.com/en-us/samples/azure-samples/iot-hub-sparkfun-thingdev-client-app/spark-fun-esp8266-client/  
  
If you use these codes directly, you will encounter two to three errors.  You have to change part of the ESP8266 SDK code  
in order to able to send telementry to Azure IOT Hub.

_____________________________________________________________________________________________________________________________________________

## Requirement  

<ul>
 <li>VS code</li>
 <li>Latest Arduino IDE</li>
 <li>Azure account (you can apply for a free one month trial)</li>
 <li>ESP8266 board</li>
</ul>

_____________________________________________________________________________________________________________________________________________

## Instruction

1. Dowload latest version of Arduino IDE (my version is 1.8.13, portable version)

2. Adding board manager of ESP8266 to the arduino idle. Below is the link to do this  
   https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/  

3.  Install the following libraries from Sketch -> Include library -> Manage libraries  
    (Remember to make sure the publisher is by Microsoft)

>AzureIoTHub (mine is version 1.3.8)  
AzureIoTUtility (mine is version 1.3.8)  
AzureIoTProtocol_MQTT (mine is version 1.3.8)  
ArduinoJson (mine is version 5.1.3) Don't use version 6.x.x some function is not available in this version  
Adafruit_MCP3008 (if you are using MCP3008)  


4. Fix the inevitable error of the ESP8266 SDK code  

>C:\Users\leolau\Documents\Arduino\libraries\AzureIoTHub\src\iothub_client_ll_uploadtoblob.c:26:27: fatal error: internal/blob.h: No such file or directory
 #include "internal/blob.h"  
                           ^  
>compilation terminated.  
exit status 1  
Error compiling for board NodeMCU 1.0 (ESP-12E Module).  

You have to change some codes inside azure library in order to make it works.
<ol>
 <ol>
 <li>Navigate to<br>
  <br>
   <em>C:\Users\Change_to_your_window_user_name\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.7.1\cores\esp8266</em></li><br>  
 <br>
 <li>Locate and open the file <em>Arduino.h</em> in VS code, comment out the line<br>
  <em>#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))</em><br>
  at line 137.</li><br>

<li>paste the platform.local.txt file from<br>
 <br>
<em>C:\Users\Change_to_your_window_user_name\Documents\Arduino\libraries\AzureIoTHub\examples\iothub_ll_telemetry_sample\esp8266</em><br>
 <br>
 into two folders' level up from the Arduino.h step above<br>
 <em>(C:\Users\Change_to_your_window_user_name\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.7.1)</em><br>
 </li>  
<br>
Open platform.txt and add <em>-DDONT_USE_UPLOADTOBLOB</em> and <em>-DUSE_BALTIMORE_CERT</em> to <em>build.extra_flags=</em> in platform.txt in order to run the sample.<br>
<br>
Note1: If your device is not intended to connect to the global portal.azure.com, please change the CERT define to the appropriate cert define as laid out in certs.c<br>
<br>
Note2: Due to RAM limits of ESP8266, you must select just one CERT define.<br>
</ol>  
</ol><br>

5.  Next Error :  

>exec: "{runtime.tools.python.path}/python": file does not exist  
Error compiling for board NodeMCU 1.0 (ESP-12E Module).

There is also an issues related to the error as 
>python.exe": file does not exist  

I follow the below instruction to fix it: https://forum.arduino.cc/index.php?topic=663500.0  

By doing this:
<ol>
 <ol>
<li>Open the platform.local.txt file you saved to the ESP8266 core in a text editor.</li>

 <li>Delete the contents of the file.</li>

<li>Add this single line of code:  
>Code: [Select]  
build.extra_flags=-DESP8266 -DDONT_USE_UPLOADTOBLOB -DUSE_BALTIMORE_CERT  
 </li>
 </ol>
</ol><br>

6.  Compile and upload the sample code of Microsoft to your ESP8266 board and it should work.  

