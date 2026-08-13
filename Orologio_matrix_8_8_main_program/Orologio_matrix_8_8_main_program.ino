#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

#define TCAADDR 0x71             // Indirizzo TCA9548A con A0 a 5V
#define DS3231_I2C_ADDRESS 0x68  // Indirizzo I2C standard del DS3231

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

// Font 8x8 per le cifre 0-9
static const uint8_t digits[10][8] = {
  {0x3C,0x42,0x46,0x4A,0x52,0x62,0x42,0x3C}, // 0
  {0x08,0x18,0x28,0x08,0x08,0x08,0x08,0x3E}, // 1
  {0x3C,0x42,0x02,0x04,0x18,0x20,0x40,0x7E}, // 2
  {0x7E,0x04,0x18,0x04,0x02,0x42,0x42,0x3C}, // 3
  {0x04,0x0C,0x14,0x24,0x44,0x7E,0x04,0x04}, // 4
  {0x7E,0x40,0x7C,0x02,0x02,0x42,0x42,0x3C}, // 5
  {0x1C,0x20,0x40,0x7C,0x42,0x42,0x42,0x3C}, // 6
  {0x7E,0x02,0x04,0x08,0x10,0x20,0x20,0x20}, // 7
  {0x3C,0x42,0x42,0x3C,0x42,0x42,0x42,0x3C}, // 8
  {0x3C,0x42,0x42,0x3E,0x02,0x02,0x04,0x38}  // 9
};

// Conversione BCD -> Decimale
uint8_t bcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}

// Seleziona il canale del TCA9548A (0..7)
void tcaSelect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Deseleziona tutti i canali del TCA per isolare il bus durante la lettura dell'RTC
void tcaDisableAll() {
  Wire.beginTransmission(TCAADDR);
  Wire.write(0x00);
  Wire.endTransmission();
}

// Lettura diretta orario da DS3231
void readDS3231time(uint8_t *second, uint8_t *minute, uint8_t *hour) {
  tcaDisableAll();
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x00); // Registro inizio lettura (secondi)
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 3);
  if (Wire.available() >= 3) {
    *second = bcdToDec(Wire.read() & 0x7F);
    *minute = bcdToDec(Wire.read());
    *hour   = bcdToDec(Wire.read() & 0x3F);
  }
}

// Rotazione 90° anticlockwise
void rotate90_CCW(const uint8_t src[8], uint8_t dst[8]) {
  for (int y = 0; y < 8; y++) {
    uint8_t row = 0;
    for (int x = 0; x < 8; x++) {
      row <<= 1;
      row |= (src[x] >> (7 - y)) & 1;
    }
    dst[y] = row;
  }
}

// Flip orizzontale
uint8_t flipByte(uint8_t b) {
  uint8_t r = 0;
  for (int i = 0; i < 8; i++) {
    r <<= 1;
    r |= (b & 1);
    b >>= 1;
  }
  return r;
}

// Disegna una cifra applicando la trasformazione sul canale specificato
void drawDigitOnChannel(uint8_t channel, uint8_t digit) {
  if (digit > 9) return;

  uint8_t rotated[8];
  uint8_t flippedH[8];
  uint8_t finalRow[8];

  rotate90_CCW(digits[digit], rotated);

  for (int i = 0; i < 8; i++)
    flippedH[i] = flipByte(rotated[i]);

  for (int i = 0; i < 8; i++)
    finalRow[i] = flippedH[7 - i];

  tcaSelect(channel);
  for (int row = 0; row < 8; row++) {
    matrix.displaybuffer[row] = finalRow[row];
  }
  matrix.writeDisplay();
}

void setup() {
  Wire.begin();
  Wire.setClock(100000);

  // Inizializza ciascun display sui canali 0..5
  for (uint8_t c = 0; c < 6; c++) {
    tcaSelect(c);
    delay(10);
    matrix.begin(0x70);
    matrix.setBrightness(4); // Luminosità a metà (da 0 a 15)
    matrix.clear();
    matrix.writeDisplay();
  }
}

void loop() {
  uint8_t second = 0, minute = 0, hour = 0;
  readDS3231time(&second, &minute, &hour);

  uint8_t digitsToDisplay[6] = {
    (uint8_t)(hour / 10),    // Canale 0: Ore Decine
    (uint8_t)(hour % 10),    // Canale 1: Ore Unità
    (uint8_t)(minute / 10),  // Canale 2: Minuti Decine
    (uint8_t)(minute % 10),  // Canale 3: Minuti Unità
    (uint8_t)(second / 10),  // Canale 4: Secondi Decine
    (uint8_t)(second % 10)   // Canale 5: Secondi Unità
  };

  for (uint8_t channel = 0; channel < 6; channel++) {
    drawDigitOnChannel(channel, digitsToDisplay[channel]);
  }

  delay(200);
}