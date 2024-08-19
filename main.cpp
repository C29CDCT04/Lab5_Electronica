/*Universidad Del Valle de Guatemala
Facultad de Ingeniería
Departamento de Electrónica, Mecatrónica y Biomédica
Electrónica Digital 2

Carlos Daniel Camacho Tista - 22690
Susan Daniela Guevara Catalán - 22519

Laboratorio No. 5 - IoT*/

/************************ Importación de librerías *******************************/
#include <Arduino.h>
#include "config.h"

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

/************************ Declaración de variables *******************************/
// Pines del potenciometro
#define Pot1 34
#define Pot2 35
// Estructura del botón
struct Button{
  const uint8_t PIN;
  volatile uint32_t numberKeyPresses;
  bool pressed;
};
// Definición del botón de envío
Button Send ={23, 0, false};
// Pines para displays de 7 segmentos
#define pinA 14
#define pinB 27
#define pinC 33
#define pinD 25 
#define pinE 26 
#define pinF 12 
#define pinG 13
// Tabla de segmentos para display de 7 segmentos
const int segmentos[16] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // B
    0b00111001, // C
    0b01011110, // D
    0b01111001, // E
    0b01110001  // F
};

/************************ Set up de los feeds *******************************/
AdafruitIO_Feed *potCanal1 = io.feed("pot1");
AdafruitIO_Feed *potCanal2 = io.feed("pot2");
AdafruitIO_Feed *productoCanal = io.feed("producto");

/************************ Prototipo de funciones *******************************/
float leerVoltaje(int pin);
String convertirAHex(int valorAnalogico);
void mostrarEn7Segmentos(String valorHex);
void enviarDatosPot1(float voltaje);
void enviarDatosPot2(float voltaje);
//void enviarProducto(float producto);
float multiplicarVoltajes(float voltaje1, float voltaje2);
void enviarMultiplicacionHex(String hex1, String hex2);
void IRAM_ATTR BTN_SEND_ISR(void);

/************************ Configuración de pines *******************************/
void setup() {

  // Empieza la conexión serial
  Serial.begin(115200);

  // Esperar a que el monitor serial abra
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // Conectando a io.adafruit.com
  io.connect();

  // Esperando a que se establezca la conexión
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Conexión establecida
  Serial.println();
  Serial.println(io.statusText());

  // Configurar pines de salida para el display
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);

  // Configurar el botón de envío como entrada
  pinMode(Send.PIN,INPUT_PULLUP);
  attachInterrupt(Send.PIN, BTN_SEND_ISR, FALLING);
}

/************************ Bucle infinito *******************************/
void loop() {
  io.run();
  
  float voltaje1 = leerVoltaje(Pot1);
  float voltaje2 = leerVoltaje(Pot2);

  /* Multiplicar los valores de voltaje1 y voltaje2
  float producto = multiplicarVoltajes(voltaje1, voltaje2);
  Serial.print("Producto de Voltaje 1 y Voltaje 2: ");
  Serial.println(producto);*/

  // Convertir a hexadecimal y mostrar en el display
  String hex1 = convertirAHex(voltaje1);
  String hex2 = convertirAHex(voltaje2);

  mostrarEn7Segmentos(hex1);
  mostrarEn7Segmentos(hex2);

  if(Send.pressed){
    // Asegura que lee cuando se presiona el botón
    Serial.printf("Button add has been pressed %u times\n",Send.numberKeyPresses);

    enviarDatosPot1(voltaje1);
    enviarDatosPot2(voltaje2);
    enviarMultiplicacionHex(hex1, hex2);  // Enviar el resultado a Adafruit IO
  }

  delay(3000);
}

/************************ Definición de funciones *******************************/
// Función para leer el voltaje de un potenciómetro
float leerVoltaje(int pin) {
    int valorAnalogico = analogRead(pin);
    float voltaje = (valorAnalogico / 4095.0) * 3.3; // Suponiendo Vref = 3.3V y resolución de 12 bits
    return voltaje;
}

// Función para enviar los datos del primer potenciómetro a Adafruit IO
void enviarDatosPot1(float voltaje) {
    Serial.print("Voltaje 1: ");
    Serial.println(voltaje);
    potCanal1->save(voltaje);
}

// Función para enviar los datos del segundo potenciómetro a Adafruit IO
void enviarDatosPot2(float voltaje) {
    Serial.print("Voltaje 2: ");
    Serial.println(voltaje);
    potCanal2->save(voltaje);
}

// Función para enviar el producto de los voltajes a Adafruit IO
void enviarProducto(float producto) {
    Serial.print("Enviando producto: ");
    Serial.println(producto);
    productoCanal->save(producto);
}

// Función para multiplicar los voltajes
float multiplicarVoltajes(float voltaje1, float voltaje2) {
    return voltaje1 * voltaje2;
}

// Función para convertir el valor analógico a hexadecimal
String convertirAHex(int valorAnalogico) {
    return String(valorAnalogico, HEX);
}

// Función para mostrar el valor en el display de 7 segmentos
void mostrarEn7Segmentos(String valorHex) {
    for (int i = 0; i < valorHex.length(); i++) {
        char caracter = valorHex.charAt(i);
        int digito;

        // Convertir el carácter a un número de 0 a 15
        if (caracter >= '0' && caracter <= '9') {
            digito = caracter - '0';
        } else if (caracter >= 'A' && caracter <= 'F') {
            digito = caracter - 'A' + 10;
        } else {
            continue; // Si es un carácter no válido, saltar
        }

        // Enviar el valor al display (suponiendo conexión directa a pines)
        digitalWrite(pinA, segmentos[digito] & 0x01);
        digitalWrite(pinB, segmentos[digito] & 0x02);
        digitalWrite(pinC, segmentos[digito] & 0x04);
        digitalWrite(pinD, segmentos[digito] & 0x08);
        digitalWrite(pinE, segmentos[digito] & 0x10);
        digitalWrite(pinF, segmentos[digito] & 0x20);
        digitalWrite(pinG, segmentos[digito] & 0x40);

        delay(1000); // Mantener el dígito/letra encendido por un segundo
    }
}

// Función para multiplicar los valores hexadecimales y enviarlo al dashboard
void enviarMultiplicacionHex(String hex1, String hex2) {
    unsigned long valor1 = strtoul(hex1.c_str(), NULL, 16);
    unsigned long valor2 = strtoul(hex2.c_str(), NULL, 16);
    unsigned long resultado = valor1 * valor2;

    String resultadoHex = String(resultado, HEX);
    Serial.print("Resultado de la multiplicación en Hex: ");
    Serial.println(resultadoHex);

    productoCanal->save(resultadoHex);
}

// Interrupción por el botón que envía
void IRAM_ATTR BTN_SEND_ISR(){
  portENTER_CRITICAL_ISR(&mux);
    Send.numberKeyPresses++;
    Send.pressed=true;
  portEXIT_CRITICAL_ISR(&mux);
}
