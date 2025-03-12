retries++;
delay(500);
Serial.print(".");
if (retries > 14) {
}
Serial.println(F("WiFi connection FAILED"));
if (WiFi.status() == WL_CONNECTED) {
Serial.println(F("WiFi connected!"));
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}
}
Serial.println(F("Setup ready"));
#define SOUND_VELOCITY 0.034 // Speed of sound in cm/microsecond

double getDistanceCM() {
    // Send a trigger pulse to the sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the duration of the echo pulse
    unsigned long duration = pulseIn(echoPin, HIGH);

    // Calculate distance in centimeters
    // Distance = (duration * speed of sound) / 2
    double distanceCm = (duration * SOUND_VELOCITY) / 2.0;

    return distanceCm;
}

void setup() {
    Serial.begin(9600);
    connectToWiFi();
    pinMode(trigPin, INPUT);
    pinMode(echoPin, INPUT);
    pinMode(relay_pump, OUTPUT);
    pinMode(relay_water, OUTPUT);
    Blynk.begin(auth, ssid, password);
    timer.setInterval(1000L, sendSensor);
    Serial.println("Setup done");
}

void loop() {
    Blynk.run();
    timer.run();
}
void sendSensor() {
    String infoPump, infoWater;
    double distance = getDistanceCM();
    Serial.print("Distance (cm) : ");
    Serial.println(distance);

    double moisture = analogRead(A0);
    moisture = map(moisture, 1024, 250, 0, 100);
    Serial.print("Moisture: ");
    Serial.println(moisture);

    Blynk.virtualWrite(V0, moisture);
    Blynk.virtualWrite(V1, distance);

    // Automatic control of the water pump based on distance
    if (distance <= 25) {
        digitalWrite(relayPump, HIGH); // Turn off the water pump
        Blynk.virtualWrite(V2, 0); // Update the button state
        infoPump = "Turning off water pump. ";
    } else if (distance > 150) {
        digitalWrite(relayPump, LOW); // Turn on the water pump
        Blynk.virtualWrite(V2, 1); // Update the button state
        infoPump = "Turning on water pump. ";
    } else {
        infoPump = "Water level is OK. ";
    }

    // Automatic watering based on soil moisture
    if (moisture <= 25) {
        digitalWrite(relayWater, LOW); // Start watering the plant
        Blynk.virtualWrite(V3, 1); // Update the button state
        infoWater = "Watering the plant. ";
    } else if (moisture > 75) {
        digitalWrite(relayWater, HIGH); // Stop watering the plant
        Blynk.virtualWrite(V3, 0); // Update the button state
        infoWater = "Stop watering the plant. ";
    } else {
        infoWater = "Soil moisture is OK. ";
    }

    Serial.println(infoPump + infoWater);
    Blynk.virtualWrite(V4, infoPump + infoWater);
}
