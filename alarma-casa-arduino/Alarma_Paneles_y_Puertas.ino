/*
 * ALARMA PARA CASA - Puerta principal y terraza
 * Por Fernando Santos - Proyecto con Arduino
 *
 * CONEXIONES:
 *   D2  -> Sensor puerta principal (reed switch a GND)
 *   D3  -> Sensor terraza (reed switch a GND)
 *   D8  -> Buzzer
 *   D9  -> LED rojo
 *   D10 -> LED amarillo
 *   D11 -> Botón armar/desarmar (a GND)
 *   LCD I2C 16x2 en dirección 0x27
 *
 * FUNCIONAMIENTO:
 *   - LED amarillo fijo = sistema ARMADO
 *   - LEDs apagados = sistema DESARMADO
 *   - Al dispararse: rojo + amarillo intermitentes + buzzer
 *   - La alarma queda ENCLAVADA hasta pulsar el botón
 *   - Al armar hay 15 segundos de retardo de salida
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================
// ENTRADAS
// =====================================
const int SENSOR_PUERTA  = 2;
const int SENSOR_TERRAZA = 3;
const int BOTON          = 11;

// =====================================
// SALIDAS
// =====================================
const int BUZZER       = 8;
const int LED_ROJO     = 9;
const int LED_AMARILLO = 10;

// =====================================
// TIEMPOS
// =====================================
const unsigned long TIEMPO_SALIDA = 15000; // 15 segundos de retardo de salida
const unsigned long TIEMPO_LED    = 300;   // parpadeo de alarma

// =====================================
// ESTADOS
// =====================================
bool sistemaArmado   = false;
bool alarmaDisparada = false;
bool contandoSalida  = false;
bool botonAnterior   = HIGH;

unsigned long inicioSalida   = 0;
unsigned long tiempoParpadeo = 0;
bool estadoParpadeo = false;

// =====================================
// SETUP
// =====================================
void setup()
{
  pinMode(SENSOR_PUERTA, INPUT_PULLUP);
  pinMode(SENSOR_TERRAZA, INPUT_PULLUP);
  pinMode(BOTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);

  noTone(BUZZER);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" ALARMA FERNAN ");
  lcd.setCursor(0, 1);
  lcd.print(" SISTEMA OFF    ");
}

// =====================================
// LOOP PRINCIPAL
// =====================================
void loop()
{
  // ===================================
  // LECTURA DEL BOTON (armar / desarmar)
  // ===================================
  bool botonActual = digitalRead(BOTON);

  if (botonAnterior == HIGH && botonActual == LOW)
  {
    delay(50); // anti-rebote

    if (digitalRead(BOTON) == LOW)
    {
      // ---------------------------------
      // SI ESTA DESARMADA -> COMENZAR CUENTA DE SALIDA
      // ---------------------------------
      if (!sistemaArmado && !contandoSalida)
      {
        contandoSalida = true;
        inicioSalida = millis();
        tiempoParpadeo = millis();
        estadoParpadeo = false;

        lcd.backlight();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" ARMANDO...     ");
        lcd.setCursor(0, 1);
        lcd.print("SALGA AHORA     ");
      }
      // ---------------------------------
      // SI YA ESTA ARMADA -> DESARMAR
      // ---------------------------------
      else if (sistemaArmado)
      {
        sistemaArmado = false;
        alarmaDisparada = false;
        contandoSalida = false;
        estadoParpadeo = false;
        noTone(BUZZER);
        digitalWrite(LED_ROJO, LOW);
        digitalWrite(LED_AMARILLO, LOW);

        lcd.backlight();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" ALARMA FERNAN ");
        lcd.setCursor(0, 1);
        lcd.print(" SISTEMA OFF    ");
      }
      delay(300);
    }
  }
  botonAnterior = botonActual;

  // ===================================
  // CUENTA DE SALIDA (15 segundos)
  // ===================================
  if (contandoSalida)
  {
    unsigned long tiempoTranscurrido = millis() - inicioSalida;

    // Parpadeo del LED amarillo durante la cuenta
    if (millis() - tiempoParpadeo >= TIEMPO_LED)
    {
      tiempoParpadeo = millis();
      estadoParpadeo = !estadoParpadeo;
      digitalWrite(LED_AMARILLO, estadoParpadeo);
      digitalWrite(LED_ROJO, LOW);
    }

    // Mostrar segundos restantes en el LCD
    int segundosRestantes = 15 - (tiempoTranscurrido / 1000);
    if (segundosRestantes < 0) segundosRestantes = 0;

    lcd.setCursor(0, 1);
    lcd.print("SALGA: ");
    lcd.print(segundosRestantes);
    lcd.print(" SEG   ");

    // Terminó la cuenta -> sistema armado
    if (tiempoTranscurrido >= TIEMPO_SALIDA)
    {
      contandoSalida = false;
      sistemaArmado = true;
      estadoParpadeo = false;
      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_AMARILLO, HIGH);

      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" ALARMA FERNAN ");
      lcd.setCursor(0, 1);
      lcd.print(" SISTEMA ARMADO ");
    }
    return;
  }

  // ===================================
  // SISTEMA DESARMADO
  // ===================================
  if (!sistemaArmado)
  {
    noTone(BUZZER);
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AMARILLO, LOW);
    return;
  }

  // ===================================
  // DETECTAR DISPARO
  // ===================================
  if (!alarmaDisparada)
  {
    if (digitalRead(SENSOR_PUERTA) == HIGH)
    {
      alarmaDisparada = true;
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ALERTA !!!");
      lcd.setCursor(0, 1);
      lcd.print("PUERTA CASA");
    }
    else if (digitalRead(SENSOR_TERRAZA) == HIGH)
    {
      alarmaDisparada = true;
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ALERTA !!!");
      lcd.setCursor(0, 1);
      lcd.print("TERRAZA");
    }
  }

  // ===================================
  // ALARMA DISPARADA (enclavada)
  // ===================================
  if (alarmaDisparada)
  {
    if (millis() - tiempoParpadeo >= TIEMPO_LED)
    {
      tiempoParpadeo = millis();
      estadoParpadeo = !estadoParpadeo;

      // ROJO + AMARILLO juntos
      digitalWrite(LED_ROJO, estadoParpadeo);
      digitalWrite(LED_AMARILLO, estadoParpadeo);

      if (estadoParpadeo) tone(BUZZER, 2200);
      else noTone(BUZZER);
    }
  }
  // ===================================
  // ARMADO SIN ALARMA
  // ===================================
  else
  {
    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AMARILLO, HIGH);
    noTone(BUZZER);
  }
}
