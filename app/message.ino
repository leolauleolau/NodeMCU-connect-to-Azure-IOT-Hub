#include <ArduinoJson.h>
#include <Adafruit_MCP3008.h>

float analog_read_loop (int chan) {

  // Serial.print(adc.readADC(chan)); Serial.print("\t");
  reading[chan] = adc.readADC(chan) * 3.3 / 1023.0;
  return reading[chan];
}

bool readMessage(int messageId, char *payload)
{
  unsigned int prev_time = millis() / 1000;
    while ((millis() / 1000) - (prev_time) <= 10)
  {
  unsigned int prev_time = 0;
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      //      Serial.print("Encoding GPS..." );
      if (gps.location.isValid() and int(millis() / 1000) > time_to_wait_GPS_steady)
      {
        //    Serial.print("Y," );Serial.print("pos," );Serial.print(pos+1);Serial.print(",");
        //gps.satellites.value();
        if (pos == 0)
        {
          GPS_filter_count = 0;
          lastTimeReceived = millis();
          Serial.print("N," );
          northing[pos] = gps.location.lat();
          Serial.print(northing[pos], 9); Serial.print(",");
          Serial.print("E," );
          easting[pos] = gps.location.lng();
          Serial.print(easting[pos], 9); Serial.print(",");
          pos ++;
          Serial.println();
        }
        else {
          //          Serial.println();
          Serial.print("N," ); northing[pos] = gps.location.lat(); Serial.print(northing[pos], 9); Serial.print(","); //need to trigger gps.location.lat()
          //          Serial.print("NA,"); Serial.print((northing[pos] - northing[pos - 1]) > GPS_north_filter); Serial.print(",");
          Serial.print("NB,"); Serial.print((northing[pos] - northing[pos - 1]), 9); Serial.print(",");
          //          Serial.print("NC,"); Serial.print(northing[pos] - stats.average(northing, pos + 1) > GPS_north_filter); Serial.print(",");
          //          Serial.print("ND,"); Serial.print(stats.average(northing, pos + 1), 9); Serial.print(",");
          //          Serial.print("NE,"); Serial.print((northing[pos] - stats.average(northing, pos + 1)), 9); Serial.print(",");
          //
          //          Serial.println();
          Serial.print("E," ); easting[pos] = gps.location.lng(); Serial.print(easting[pos], 9); Serial.print(","); //need to trigger gps.location.lng()
          //          Serial.print("EA,"); Serial.print((easting[pos] - easting[pos - 1]) > GPS_east_filter); Serial.print(",");
          Serial.print("EB,"); Serial.print((easting[pos] - easting[pos - 1]), 9); Serial.print(",");
          //          Serial.print("EC,"); Serial.print(easting[pos] - stats.average(easting, pos + 1) > GPS_east_filter); Serial.print(",");
          //          Serial.print("ED,"); Serial.print(stats.average(easting, pos + 1), 9); Serial.print(",");
          //          Serial.print("EE,"); Serial.print((easting[pos] - stats.average(easting, pos + 1)), 9); Serial.print(",");
          Serial.println();
          if (((northing[pos] - northing[pos - 1]) < GPS_north_filter) and (northing[pos] - stats.average(northing, pos + 1) < GPS_north_filter) and northing[pos] > north_bound) {
            if (((easting[pos] - easting[pos - 1]) < GPS_east_filter) and (easting[pos] - stats.average(easting, pos + 1) < GPS_east_filter) and easting[pos] > east_bound) {
              pos++;
            }
            else {
              //          Serial.print("E_Filtered," );
              GPS_filter_count ++;
              //          Serial.print("filterCount," );Serial.print(GPS_filter_count);Serial.println();
            }
          }
          else {
            //        Serial.print("N_Filtered," );
            GPS_filter_count ++;
            //        Serial.print("filterCount," );Serial.print(GPS_filter_count);Serial.println();
          }
        }
        //    Serial.println();
        if (GPS_filter_count > GPS_filter_threshold) {
          pos = 0;
          GPS_filter_count = 0;
        }
      }
      else
      {
        //    Serial.println("N," );
      }
  delay(200);
  }
    long elapsed = millis() - lastTimeReceived;
         if (pos == 0) {
      northing_final = 0;
      easting_final = 0;
    }
    else {
      northing_final = stats.average(northing, pos);
      easting_final = stats.average(easting, pos);
    }

    Serial.print("N,"); Serial.print(northing_final, 9); Serial.print(",");
    Serial.print("E,"); Serial.print(easting_final, 9); Serial.print(",");

    float T1 = analog_read_loop (0);
    float T2 = analog_read_loop (1);
    float T3 = analog_read_loop (2);
    float P1 = analog_read_loop (3);
    float P2 = analog_read_loop (4);
    float P3 = analog_read_loop (5);
    float P4 = analog_read_loop (6);
    float P5 = analog_read_loop (7);

    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;   // You have to downgrade the ArduinoJson library to 5.13 to elimilate the not declare error
    JsonObject &root = jsonBuffer.createObject();
    root["devicegroup"] = "esp8266";
    root["device"] = DEVICE_ID;
    root["time"] = millis() / 1000;
    root["messageId"] = messageId;
    root["N"] = northing_final;
    root["E"] = easting_final;
    root["T1"] = T1;
    root["T2"] = T2;
    root["T3"] = T3;
    root["P1"] = P1;
    root["P2"] = P2;
    root["P3"] = P3;
    root["P4"] = P4;
    root["P5"] = P5;
    root["P6"]= analogRead(A0)*3.3/1023.0;


    root.printTo(payload, MESSAGE_MAX_LEN);
    prev_time = millis() / 1000;
  }
   


void parseTwinMessage(char *message)
{
  StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(message);
  if (!root.success())
  {
    Serial.printf("Parse %s failed.\r\n", message);
    return;
  }

  if (root["desired"]["interval"].success())
  {
    interval = root["desired"]["interval"];
  }
  else if (root.containsKey("interval"))
  {
    interval = root["interval"];
  }
}
