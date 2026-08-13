#include <Wire.h>

#define DS3231_I2C_ADDRESS 0x68

// Conversione Decimale -> Binary Coded Decimal (BCD)
uint8_t decToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

// Conversione BCD -> Decimale
uint8_t bcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}

// Converte i tre caratteri del mese (es. "Aug") nel rispettivo numero (1-12)
uint8_t parseMonth(const char *monthStr) {
  switch (monthStr[0]) {
    case 'J': return (monthStr[1] == 'a') ? 1 : ((monthStr[2] == 'n') ? 6 : 7);
    case 'F': return 2;
    case 'M': return (monthStr[2] == 'r') ? 3 : 5;
    case 'A': return (monthStr[1] == 'p') ? 4 : 8;
    case 'S': return 9;
    case 'O': return 10;
    case 'N': return 11;
    case 'D': return 12;
  }
  return 1;
}

// Funzione per impostare orario e data nel DS3231
void setDS3231time(uint8_t second, uint8_t minute, uint8_t hour, 
                    uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x00); // Imposta il puntatore dei registri a 00h
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek)); // 1=Domenica, 2=Lunedì, ... 7=Sabato
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year)); // ultime 2 cifre (es. 26 per 2026)
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Estrazione automatica da __DATE__ ("MMM DD YYYY") e __TIME__ ("HH:MM:SS")
  char monthStr[4];
  int day, year, hour, minute, second;

  sscanf(__DATE__, "%s %d %d", monthStr, &day, &year);
  sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);

  uint8_t month = parseMonth(monthStr);
  uint8_t shortYear = year % 100; // Es. 2026 -> 26

  // Imposta l'orario estratto al momento della compilazione
  // (Usa 1 come giorno della settimana generico)
  setDS3231time(second, minute, hour, 1, day, month, shortYear);

  Serial.println("--- RTC DS3231 AGGIORNATO ---");
  Serial.print("Data impostata: ");
  Serial.print(day); Serial.print("/");
  Serial.print(month); Serial.print("/");
  Serial.println(year);

  Serial.print("Ora impostata:  ");
  if (hour < 10) Serial.print("0");
  Serial.print(hour); Serial.print(":");
  if (minute < 10) Serial.print("0");
  Serial.print(minute); Serial.print(":");
  if (second < 10) Serial.print("0");
  Serial.println(second);
}

void loop() {
  // Lettura e verifica continua sul Monitor Seriale
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 3);
  if (Wire.available() >= 3) {
    uint8_t s = bcdToDec(Wire.read() & 0x7F);
    uint8_t m = bcdToDec(Wire.read());
    uint8_t h = bcdToDec(Wire.read() & 0x3F);

    Serial.print("Ora attuale RTC: ");
    if (h < 10) Serial.print("0"); Serial.print(h); Serial.print(":");
    if (m < 10) Serial.print("0"); Serial.print(m); Serial.print(":");
    if (s < 10) Serial.print("0"); Serial.println(s);
  }

  delay(1000);
}