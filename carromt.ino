#include <SPI.h>
#include <Ethernet.h>

// Configuración de la dirección MAC y la IP estática para el Ethernet Shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 177);  // Configura la IP según tu red local
IPAddress server(192, 168, 0, 1); // Dirección IP del servidor XAMPP
EthernetServer serverHTTP(80);    // Servidor para recibir comandos en puerto 80
EthernetClient client;

// Puerto del LM35
const int LM35_PIN = A0;

// Pines del puente H para los motores
int ENA = 3;  // Control de velocidad del Motor 1 (PWM)
int IN1 = 5;  // Dirección 1 del Motor 1
int IN2 = 6;  // Dirección 2 del Motor 1

int ENB = 11; // Control de velocidad del Motor 2 (PWM)
int IN3 = 9;  // Dirección 1 del Motor 2
int IN4 = 7;  // Dirección 2 del Motor 2

unsigned long lastTempSend = 0;  // Temporizador para enviar temperatura
const unsigned long interval = 10000;  // Intervalo de 10 segundos

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  serverHTTP.begin();  // Inicia el servidor
  Serial.print("Servidor iniciado en: ");
  Serial.println(Ethernet.localIP());

  // Configurar pines del puente H
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // 1. Control de motores desde la red
  EthernetClient clientHTTP = serverHTTP.available();
  if (clientHTTP) {
    String request = "";  // Almacena la solicitud del cliente
    while (clientHTTP.connected()) {
      if (clientHTTP.available()) {
        char c = clientHTTP.read();
        request += c;
        if (c == '\n') break;  // Fin de la solicitud HTTP
      }
    }

    // Analiza los datos recibidos
    if (request.indexOf("GET /?data=") != -1) {
      int dataIndex = request.indexOf("data=") + 5;
      String dataValue = request.substring(dataIndex, dataIndex + 1);
      Serial.print("Dato recibido: ");
      Serial.println(dataValue.charAt(0));
      processCommand(dataValue.charAt(0));
    }

    // Respuesta al cliente
    clientHTTP.println("HTTP/1.1 200 OK");
    clientHTTP.println("Content-Type: text/html");
    clientHTTP.println();
    clientHTTP.println("<h1>Dato recibido correctamente</h1>");
    clientHTTP.stop();
  }

  // 2. Envío periódico de temperatura al servidor
  if (millis() - lastTempSend >= interval) {
    lastTempSend = millis();
    float temperatura = leerTemperatura();
    enviarTemperatura(temperatura);
  }
}

// Función para leer la temperatura del LM35
float leerTemperatura() {
  int lectura = analogRead(LM35_PIN);        // Lee el sensor LM35
  float voltaje = (lectura / 1024.0) * 5.0;  // Convierte la lectura a voltaje
  float tempF = voltaje * 100;               // Convierte a grados Fahrenheit
  
  // Convertir de Fahrenheit a Celsius
  float tempC = (tempF - 32) * 5.0 / 9.0;

  Serial.print("Temperatura en Fahrenheit: ");
  Serial.print(tempF);
  Serial.println(" °F");

  Serial.print("Temperatura en Celsius: ");
  Serial.print(tempC);
  Serial.println(" °C");

  return tempC;  // Devuelve la temperatura en Celsius
}


// Función para enviar la temperatura al servidor
void enviarTemperatura(float temperatura) {
  if (client.connect(server, 80)) {
    Serial.println("Enviando temperatura al servidor...");
    String url = "/guardar.php?temp=" + String(temperatura);
    client.print("GET " + url + " HTTP/1.1\r\n");
    client.print("Host: 192.168.0.1\r\n");  // Dirección del servidor XAMPP
    client.print("Connection: close\r\n\r\n");
    client.stop();
    Serial.println("Temperatura enviada correctamente");
  } else {
    Serial.println("Error al conectar con el servidor para enviar temperatura");
  }
}

// Procesamiento de comandos de control
void processCommand(char command) {
  Serial.print("Comando recibido: ");
  Serial.println(command);
  switch (command) {
    case 'F': Adelante(); Serial.println("Motores avanzando"); break;
    case 'B': Atras(); Serial.println("Motores retrocediendo"); break;
    case 'L': Izquierda(); Serial.println("Giro a la izquierda"); break;
    case 'R': Derecha(); Serial.println("Giro a la derecha"); break;
    case 'S': Parar(); Serial.println("Motores detenidos"); break;
    default: Serial.println("Comando desconocido"); Parar(); break;
  }
}

// Funciones de control de motores
void Adelante() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); analogWrite(ENA, 255);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); analogWrite(ENB, 255);
}
void Derecha() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); analogWrite(ENA, 255);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); analogWrite(ENB, 255);
}
void Izquierda() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); analogWrite(ENA, 255);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); analogWrite(ENB, 255);
}
void Atras() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); analogWrite(ENA, 255);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); analogWrite(ENB, 255);
}
void Parar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); analogWrite(ENA, 0);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); analogWrite(ENB, 0);
}

