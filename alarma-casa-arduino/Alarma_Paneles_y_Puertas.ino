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

const int RELE_CORNETA  = 5;
const int BUZZER        = 8;
const int LED_ROJO      = 9;
const int LED_AMARILLO  = 10;

// =====================================
// TIEMPOS
// =====================================

const unsigned long TIEMPO_SALIDA = 15000; // 15 segundos
const unsigned long TIEMPO_LED = 300;

// Corneta: 1 segundo ON / 1 segundo OFF
const unsigned long TIEMPO_CORNETA = 1000;

// =====================================
// ESTADOS
// =====================================

bool sistemaArmado = false;
bool alarmaDisparada = false;
bool contandoSalida = false;

bool botonAnterior = HIGH;

unsigned long inicioSalida = 0;
unsigned long tiempoParpadeo = 0;

// Temporizador de la corneta
unsigned long tiempoCorneta = 0;

bool estadoParpadeo = false;
bool estadoCorneta = false;


// =====================================
// SETUP
// =====================================

void setup()
{
  pinMode(SENSOR_PUERTA, INPUT_PULLUP);
  pinMode(SENSOR_TERRAZA, INPUT_PULLUP);

  pinMode(BOTON, INPUT_PULLUP);

  pinMode(RELE_CORNETA, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);

  // Todo apagado al comenzar
  digitalWrite(RELE_CORNETA, LOW);

  noTone(BUZZER);

  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(" ALARMA PANELES ");

  lcd.setCursor(0, 1);
  lcd.print(" SISTEMA OFF    ");
}


// =====================================
// LOOP
// =====================================

void loop()
{
  // ===================================
  // LECTURA DEL BOTON
  // ===================================

  bool botonActual = digitalRead(BOTON);

  if (botonAnterior == HIGH && botonActual == LOW)
  {
    delay(50);

    if (digitalRead(BOTON) == LOW)
    {
      // ---------------------------------
      // SI ESTA DESARMADA
      // COMENZAR CUENTA DE SALIDA
      // ---------------------------------

      if (!sistemaArmado && !contandoSalida)
      {
        contandoSalida = true;
        inicioSalida = millis();

        tiempoParpadeo = millis();
        estadoParpadeo = false;

        // Corneta apagada durante la cuenta
        digitalWrite(RELE_CORNETA, LOW);
        estadoCorneta = false;

        lcd.backlight();
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print(" ARMANDO...     ");

        lcd.setCursor(0, 1);
        lcd.print("SALGA AHORA     ");
      }

      // ---------------------------------
      // SI YA ESTA ARMADA
      // DESARMAR
      // ---------------------------------

      else if (sistemaArmado)
      {
        sistemaArmado = false;
        alarmaDisparada = false;
        contandoSalida = false;

        estadoParpadeo = false;

        // Apagar corneta
        digitalWrite(RELE_CORNETA, LOW);
        estadoCorneta = false;

        noTone(BUZZER);

        digitalWrite(LED_ROJO, LOW);
        digitalWrite(LED_AMARILLO, LOW);

        lcd.backlight();
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print(" ALARMA 2026 ");

        lcd.setCursor(0, 1);
        lcd.print(" SISTEMA OFF    ");
      }

      delay(300);
    }
  }

  botonAnterior = botonActual;


  // ===================================
  // CUENTA DE SALIDA
  // ===================================

  if (contandoSalida)
  {
    unsigned long tiempoTranscurrido = millis() - inicioSalida;

    // ---------------------------------
    // PARPADEO DEL LED AMARILLO
    // ---------------------------------

    if (millis() - tiempoParpadeo >= TIEMPO_LED)
    {
      tiempoParpadeo = millis();

      estadoParpadeo = !estadoParpadeo;

      digitalWrite(LED_AMARILLO, estadoParpadeo);
      digitalWrite(LED_ROJO, LOW);
    }

    // ---------------------------------
    // MOSTRAR CUENTA RESTANTE
    // ---------------------------------

    int segundosRestantes =
      15 - (tiempoTranscurrido / 1000);

    if (segundosRestantes < 0)
    {
      segundosRestantes = 0;
    }

    lcd.setCursor(0, 1);
    lcd.print("SALGA: ");
    lcd.print(segundosRestantes);
    lcd.print(" SEG   ");


    // ---------------------------------
    // TERMINO LA CUENTA
    // ---------------------------------

    if (tiempoTranscurrido >= TIEMPO_SALIDA)
    {
      contandoSalida = false;
      sistemaArmado = true;

      estadoParpadeo = false;

      // Corneta apagada al quedar armado
      digitalWrite(RELE_CORNETA, LOW);
      estadoCorneta = false;

      digitalWrite(LED_ROJO, LOW);
      digitalWrite(LED_AMARILLO, HIGH);

      lcd.backlight();
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print(" ALARMA 2026 ");

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

    digitalWrite(RELE_CORNETA, LOW);
    estadoCorneta = false;

    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AMARILLO, LOW);

    return;
  }


  // ===================================
  // DETECTAR ALARMA
  // ===================================

  if (!alarmaDisparada)
  {
    // ---------------------------------
    // PUERTA PRINCIPAL
    // ---------------------------------

    if (digitalRead(SENSOR_PUERTA) == HIGH)
    {
      alarmaDisparada = true;

      // Encender corneta inmediatamente
      estadoCorneta = true;
      digitalWrite(RELE_CORNETA, HIGH);
      tiempoCorneta = millis();

      lcd.backlight();
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("!!! ALERTA !!!");

      lcd.setCursor(0, 1);
      lcd.print("PUERTA CASA");
    }

    // ---------------------------------
    // TERRAZA
    // ---------------------------------

    else if (digitalRead(SENSOR_TERRAZA) == HIGH)
    {
      alarmaDisparada = true;

      // Encender corneta inmediatamente
      estadoCorneta = true;
      digitalWrite(RELE_CORNETA, HIGH);
      tiempoCorneta = millis();

      lcd.backlight();
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("!!! ALERTA !!!");

      lcd.setCursor(0, 1);
      lcd.print("TERRAZA");
    }
  }


  // ===================================
  // ALARMA DISPARADA
  // ===================================

  if (alarmaDisparada)
  {
    // ---------------------------------
    // CORNETA
    // 1 SEGUNDO ON / 1 SEGUNDO OFF
    // ---------------------------------

    if (millis() - tiempoCorneta >= TIEMPO_CORNETA)
    {
      tiempoCorneta = millis();

      estadoCorneta = !estadoCorneta;

      digitalWrite(RELE_CORNETA, estadoCorneta);
    }


    // ---------------------------------
    // BUZZER + LEDS
    // ---------------------------------

    if (millis() - tiempoParpadeo >= TIEMPO_LED)
    {
      tiempoParpadeo = millis();

      estadoParpadeo = !estadoParpadeo;

      // ROJO + AMARILLO
      // LOS DOS JUNTOS = NARANJA

      digitalWrite(LED_ROJO, estadoParpadeo);
      digitalWrite(LED_AMARILLO, estadoParpadeo);

      if (estadoParpadeo)
      {
        tone(BUZZER, 2200);
      }
      else
      {
        noTone(BUZZER);
      }
    }
  }


  // ===================================
  // ARMADO SIN ALARMA
  // ===================================

  else
  {
    digitalWrite(RELE_CORNETA, LOW);
    estadoCorneta = false;

    digitalWrite(LED_ROJO, LOW);
    digitalWrite(LED_AMARILLO, HIGH);

    noTone(BUZZER);
  }
}
