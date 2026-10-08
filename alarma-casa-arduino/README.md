# Alarma para casa con Arduino

Alarma con sensores de puerta principal y terraza, pantalla LCD I2C, buzzer y LEDs indicadores.
Código de los videos del canal de YouTube.

## Conexiones

| Pin | Componente |
|-----|-----------|
| D2  | Sensor puerta principal (reed switch a GND) |
| D3  | Sensor terraza (reed switch a GND) |
| D8  | Buzzer |
| D9  | LED rojo |
| D10 | LED amarillo |
| D11 | Botón armar/desarmar (a GND) |
| I2C | LCD 16x2 dirección 0x27 |

## 📌 Resumen de conexiones

- 🏠 D2 → puerta principal
- 🌴 D3 → terraza
- 🔊 D8 → buzzer
- 🔴 D9 → LED rojo
- 🟡 D10 → LED amarillo
- 🔘 D11 → botón armar/desarmar
- 🟡 Sistema armado → amarillo fijo
- 🟠 Alarma disparada → rojo + amarillo intermitentes
- 🔒 Alarma enclavada (no se apaga sola)
- ⏱️ Retardo de salida → 15 segundos
- 📟 LCD funcionando

## Funcionamiento

- 🟡 LED amarillo fijo = sistema **ARMADO**
- ⚫ LEDs apagados = sistema **DESARMADO**
- 🟠 Rojo + amarillo intermitentes + buzzer = **ALARMA DISPARADA**
- 🔒 La alarma queda **enclavada** hasta pulsar el botón, aunque cierres la puerta
- ⏱️ Al armar hay **15 segundos** de retardo de salida (el LCD muestra la cuenta atrás)

## Librerías necesarias (Arduino IDE)

- `LiquidCrystal_I2C`
- `Wire` (incluida en Arduino)

## Archivo

- `Alarma_Paneles_y_Puertas.ino` — versión final probada y funcionando
