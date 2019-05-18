//Entfernungsmessung mittels Ultraschallsensor
//Rückfahrwarner durch Aufleuchten der internen LED bei Unterschreitung eines definierten Abstandes und Ertönen der CDur-Tonleiter (aufwärts) durch einen Piezo-Lautsprecher.
//Aufzeichnung der Distanz im JSON-Format, Ausgabe der Entfernungswerte in CM mit einer Kommastelle, da der Ultraschallsensor auf 3mm genau messen kann. Diese Werte werden in Folge auf einen Integer gerundet.
//Die optimale Messdistanz des Ultraschallsensores liegt zwischen ca 2 cm und 300 cm. Bei Unterschreitung der definierten Messdistanz erfolgt die Mitgabe einer Fehlermeldung mittels "False", wenn die Messung korrekt
//abläuft wird "True" mitgegeben (max_distance des Sensors bleibt im BSP unberücksichtigt, da für angenommene Problemstellung nicht relevant).

#include <ArduinoJson.h>

int trigger_pin = 7; // Trigger-Pin des Ultraschallsensors an Pin7 des Arduino-Boards.
int echo_pin = 9; // Echo-Pin des Ultraschallsensors an Pin6 des Arduino-Boards.
int piezo = 5; // Pin des Piezo-Lautsprechers.
int min_distance = 30; //Definierte Minimum-Distanz. 
long dauer = 0; // Speicherung der Zeit, die eine Schallwelle bis zur Reflektion und zurück benötigt, Startwert ist 0. 
float entfernung = 0; // Speicherung der Entfernung zum Objekt.
long entfernungGerundet = 0; //Runden der Entfernung auf eine Ganzzahl, long kann größere Zahlen speichen.
long previousMillis = 0; // Speichert die Zeit des letzten Updates.
long interval = 1000; //Zeitlich definiertes Intervall, in dem der Sensor gestartet/ausgelesen werden soll.

//Funktion zur Ausgabe des C-Dur-Tonleiter, tone(Pin, Tonhöhe in hz).
void cDur(){
      tone(5, 264);
      delay(100);
      noTone(5);
      tone(5, 297);
      delay(100);
      noTone(5);
      tone(5, 330);
      delay(100);
      noTone(5);
      tone(5, 352);
      delay(100);
      noTone(5);
      tone(5, 396);
      delay(100);
      noTone(5);
      tone(5, 440);
      delay(100);
      noTone(5);
      tone(5, 495);
      delay(100);
      noTone(5);
      tone(5, 528);
      delay(100);
      noTone(5);
}

void setup()
{
  Serial.begin (9600); //Serielle Kommunikation starten, um die Werte am Serial Monitor ansehen zu können.
  pinMode(trigger_pin, OUTPUT); // Trigger-Pin ist ein Ausgang.
  pinMode(echo_pin, INPUT); // Echo-Pin ist ein Eingang.
  pinMode(LED_BUILTIN, OUTPUT); //Definition der internen LED als Ausgang.
  pinMode(piezo, OUTPUT); //Definition des Piezo-Lautsprecher an Pin5 als Ausgang.
}

void loop()
{
  DynamicJsonDocument doc(200); //serialisiert Objekt zu JSON-Dokument
  
  unsigned long currentMillis = millis();
  //Legt zeitlichen Rahmen fest, an dem Messung wieder beginnen soll.
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    
    digitalWrite(trigger_pin, LOW); //Spannung wird für kurze Zeit vom Trigger-Pin genommen - sorgt für rauschfreies Signal.
    delay(5); //Dauer: 5 Millisekunden
    digitalWrite(trigger_pin, HIGH); //Ultraschallwellen/Ton werden losgesendet.
    delay(10); //Dauer: 10 Millisekunden
    digitalWrite(trigger_pin, LOW);//Ultraschallwellen/Ton werden abgeschalten.
    dauer = pulseIn(echo_pin, HIGH); //Mit dem Befehl „pulseIn“ zählt der Mikrokontroller die Zeit in Mikrosekunden, bis der Schall zum Ultraschallsensor zurückkehrt.
    entfernung = (round((dauer / 2) * 0.03432*10))/10.0; //Berechnung der Entfernung in cm mit einer Kommastelle. Teilung durch 2, da Weg zum Objekt und zurück. Multiplikation mit der Schallgeschwindigkeit in der Einheit Zentimeter/Mikrosekunde und erhält dann den Wert in Zentimetern.
    entfernungGerundet = round(entfernung); //Runden der Entfernung auf eine Ganzzahl
    
    if (entfernung <= min_distance) 
    {
      doc["Messung"] = "False";
      doc["DauerInMs"] = dauer;
      doc["EntfernungInCm"] = entfernung;
      doc["EntfernungIntInCm"] = entfernungGerundet;
      //serializeJson(doc, Serial);
      //Serial.println();
      serializeJsonPretty(doc, Serial);
      digitalWrite(LED_BUILTIN, HIGH); //Einschalten der internen LED durch erhöhen der Volt.
      cDur(); //Aufrufen der Funktion.
    }
    else
    {
      noTone(5); //Ausschalten des Tons.
      digitalWrite(LED_BUILTIN, LOW); //Ausschalten der interenen LED durch ausschalten der Volt
      
      doc["Messung"] = "True";
      doc["DauerInMs"] = dauer;
      doc["EntfernungInCm"] = entfernung;
      doc["EntfernungIntInCm"] = entfernungGerundet;

      //serializeJson(doc, Serial);
      //Serial.println();
      serializeJsonPretty(doc, Serial);
    }
  }
}
