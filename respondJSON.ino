#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

struct LedButton {
  int ledPin;
  String onUrl;
  String offUrl;
  String statusUrl;
};

const char* ssid = "NamaSSID";
const char* password = "NamaPswd";

ESP8266WebServer server(80);

LedButton ledsButtons[] = {
  {D5, "/LED/1/ON", "/LED/1/OFF", "/LED/1/STATUS"},
  {D6, "/LED/2/ON", "/LED/2/OFF", "/LED/2/STATUS"},
  {D7, "/LED/3/ON", "/LED/3/OFF", "/LED/3/STATUS"},
  // Tambahkan baris ini untuk setiap LED dan tombol tambahan
};

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  for (auto& lb : ledsButtons) {
    pinMode(lb.ledPin, OUTPUT);
    server.on(lb.onUrl, HTTP_GET, [lb]() {
      digitalWrite(lb.ledPin, HIGH);
      sendStatus(lb, server.uri());
    });
    server.on(lb.offUrl, HTTP_GET, [lb]() {
      digitalWrite(lb.ledPin, LOW);
      sendStatus(lb, server.uri());
    });
    server.on(lb.statusUrl, HTTP_GET, [lb]() {
      sendStatus(lb, server.uri());
    });

    // Menambahkan penanganan untuk metode HTTP POST
    server.on(lb.onUrl, HTTP_POST, [lb]() {
      digitalWrite(lb.ledPin, HIGH);
      sendStatus(lb, server.uri());
    });

    server.on(lb.offUrl, HTTP_POST, [lb]() {
      digitalWrite(lb.ledPin, LOW);
      sendStatus(lb, server.uri());
    });
  }

  // Tambahkan penanganan permintaan untuk root ("/")
  server.on("/", HTTP_GET, []() {
    sendStatus(ledsButtons[0], server.uri());
  });

  server.begin();
  Serial.println("Web server dijalankan");
}

void loop() {
  server.handleClient();
}

void sendStatus(const LedButton& lb, const String& requestUrl) {
  String status = (digitalRead(lb.ledPin) == HIGH) ? "HIGH" : "LOW";

  // Menghasilkan objek JSON dengan informasi status, pin, dan URL permintaan
  String jsonResponse = "{\"status\":\"" + status + "\",\"ledPin\":" + String(lb.ledPin) +
                        ",\"requestUrl\":\"" + requestUrl + "\"}";

  // Mengirim respons sebagai JSON
  server.send(200, "application/json", jsonResponse);
}
