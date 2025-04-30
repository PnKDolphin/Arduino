class DELAY {
  private:
    unsigned long previous_time;
    unsigned long interval;
    bool running;

  public:
    DELAY(unsigned long interval_ms) {
      interval = interval_ms;
      previous_time = 0;
      running = false;
    }

    void start() {
      previous_time = millis();
      running = true;
    }

    void stop() {
      running = false;
    }

    bool isReady() {
      if (!running) return false; // Don't do anything if timer is not running

      unsigned long current_time = millis();
      if (current_time - previous_time >= interval) {
        previous_time = current_time;
        return true;
      }
      return false;
    }

    void reset() {
      previous_time = millis();
    }

    void setInterval(unsigned long interval_ms) {
      interval = interval_ms;
    }

    bool isRunning() {
      return running;
    }
};

#include <string.h>
#include <MFRC522.h>
#include <SPI.h>

#define RED           7
#define GREEN         8

#define SEGMENT_CLOCK 39
#define SEGMENT_LATCH 38
#define SEGMENT_DATA  37

#define BUZZER        6

#define RST_PIN       5
#define SS_PIN        53

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key = {
  .keyByte = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

const int INPUT_PINS[6] = {44, 45, 46, 47, 41, 40};
const int SEGMENT_DIGITS[4] = {12, 11, 10, 9};

int LAST_INPUT_STATE[6] = {LOW, LOW, LOW, LOW, LOW, LOW};
int VALUES[4] = {0, 0, 0, 0};

const int PASSKEY[4] = {2, 0, 2, 4};

int DISPLAY_ACTIVE = 1;

DELAY Submit_LED_Delay(750);
DELAY LED_Flash_Delay(100);
bool LIGHT_ACTIVE = false;

int counter = 0;
int counter_limit = 6;

unsigned char DIGIT_TABLE[] = 
{
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x00  // BLANK (Index 10)
};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(SEGMENT_LATCH, OUTPUT);
  pinMode(SEGMENT_CLOCK, OUTPUT);
  pinMode(SEGMENT_DATA,  OUTPUT);

  pinMode(INPUT_PINS[0], INPUT_PULLUP);
  pinMode(INPUT_PINS[1], INPUT_PULLUP);
  pinMode(INPUT_PINS[2], INPUT_PULLUP);
  pinMode(INPUT_PINS[3], INPUT_PULLUP);
  pinMode(INPUT_PINS[4], INPUT_PULLUP);
  pinMode(INPUT_PINS[5], INPUT_PULLUP);

  pinMode(GREEN,  OUTPUT);
  pinMode(RED,    OUTPUT);
  pinMode(BUZZER, OUTPUT);

  DISPLAY_ACTIVE = 0;
}

void loop() 
{
  if(DISPLAY_ACTIVE == 0)
  {
    if(!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    {
      return;
    }

    byte block_id = 4;
    byte block_str = 5;
    byte buffer[18];
    byte size = sizeof(buffer);

    if(authenticateAndRead(block_id, buffer))
    {
      uint32_t id_number = 
        ((uint32_t)buffer[0] << 24) |
        ((uint32_t)buffer[1] << 16) |
        ((uint32_t)buffer[2] << 8 ) |
        ((uint32_t)buffer[3]);
      Serial.println(DISPLAY_ACTIVE);
      DISPLAY_ACTIVE = 1;
      digitalWrite(GREEN, HIGH);
      Display(1);
      Serial.println(DISPLAY_ACTIVE);
    } 

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  else if(DISPLAY_ACTIVE == 1)
  {
    for(int i = 0; i < 6; i++)
    {
      int CURRENT_INPUT_STATE = digitalRead(INPUT_PINS[i]);

      if (LAST_INPUT_STATE[i] == HIGH && CURRENT_INPUT_STATE == LOW) ButtonPress(i);
      // Save the current state for next loop
      LAST_INPUT_STATE[i] = CURRENT_INPUT_STATE;
    }

    for(int i = 0; i < 4; i++)
    {
        Display(VALUES[i]);
        SetDigit(i);
        delay(1);
    }
  }

  if(Submit_LED_Delay.isReady())
  {
    DISPLAY_ACTIVE = 0;
    digitalWrite(GREEN, LOW);
    digitalWrite(BUZZER, LOW);
  }
  if(LED_Flash_Delay.isReady())
  {
    if(counter < counter_limit)
    {
     counter++;
     LED_Flash();
    }
  }
  
}

void ButtonPress(int button)
{
  if(button < 4)
  {
    VALUES[button]++;
    if(VALUES[button] > 9) VALUES[button] = 0;
  }
  else if(button == 4)
  {
    // Submit Logic
    SubmitPasskey();
  }
  else if(button == 5)
  {
    // Reset Logic
    ResetPasskey();
  }
}

void SubmitPasskey()
{
    if(memcmp(VALUES, PASSKEY, sizeof(VALUES)) == 0)
    {
      Serial.println("Yes!");
      analogWrite(GREEN, 100);
      analogWrite(BUZZER, 10);
      Submit_LED_Delay.reset();
      Submit_LED_Delay.start();
    }
    else
    {
      Serial.println("Nope!");
      for(int i = 0; i < 4; i++)
      {
        VALUES[i] = 0;
      }
      counter = 0;
      LED_Flash();
    }
}

void ResetPasskey()
{
  for(int i = 0; i < 4; i++)
  {
    VALUES[i] = 0;
  }
}

// Selects Which Display Is Active
void SetDigit(int digit)
{
  if(digit < 4)
  {
    for(int i = 0; i < 4; i++)
    {
      if(i  == digit)
      {
        digitalWrite(SEGMENT_DIGITS[i], HIGH);
      }
      else
      {
        digitalWrite(SEGMENT_DIGITS[i], LOW);
      }
    }
  }
  else
  {
    for(int i = 0; i < 4; i++)
    {
      digitalWrite(SEGMENT_DIGITS[i], HIGH);
    }
  }
}

// Sets Number Shown On Displays
void Display(int num)
{
  digitalWrite(SEGMENT_LATCH, LOW);
  shiftOut(SEGMENT_DATA, SEGMENT_CLOCK, MSBFIRST, DIGIT_TABLE[num]);
  digitalWrite(SEGMENT_LATCH, HIGH);
}

void LED_Flash()
{
  if(counter < counter_limit)
  {
    if(LIGHT_ACTIVE == false)
    {
      analogWrite(RED, 100);
      LED_Flash_Delay.reset();
      LED_Flash_Delay.start();
      LIGHT_ACTIVE = true;
      digitalWrite(BUZZER, HIGH);
    }
    else
    {
      digitalWrite(BUZZER,LOW);
      digitalWrite(RED, LOW);
      LED_Flash_Delay.reset();
      LED_Flash_Delay.start();
      LIGHT_ACTIVE = false;
    }
  }
}

bool authenticateAndRead(byte block, byte buffer[18])
{
  MFRC522::StatusCode status;

  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
  if(status != MFRC522::STATUS_OK) return false;
  
  byte size = 18;

  status = rfid.MIFARE_Read(block, buffer, &size);
  if(status != MFRC522::STATUS_OK) return false;

  return true;
}