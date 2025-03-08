   #include <Keypad.h>
  #include <string.h>

    #define RED     50
    #define GREEN   51
    #define BLUE    52

    #define CLOCK   40
    #define LATCH   41
    #define DATA    42

    #define DIGIT_1 31
    #define DIGIT_2 32
    #define DIGIT_3 33
    #define DIGIT_4 34

    const byte ROWS = 4;
    const byte COLS = 4;

    const char password[4] = "1234";
    char attempt[5] = "";
    int current_digit = 0;     
    int digits_in_key;

    char hexaKeys[ROWS][COLS] = {
      {'1','2','3','A'},
      {'4','5','6','B'},
      {'7','8','9','C'},
      {'*','0','#','D'}
    };

    unsigned char table[]=
    {0x3f/*0*/,0x06/*1*/,0x5b/*2*/,0x4f/*3*/,0x66/*4*/,0x6d/*5*/,
    0x7d/*6*/,0x07/*7*/,0x7f/*8*/,0x6f/*9*/,0x77/*A*/,0x7c/*B*/,
    0x39/*C*/,0x5e/*D*/,0x79/*E*/,0x71/*F*/,0x00/*BLANK*/};
    
    byte rowPins[ROWS] = {29, 28, 27, 26};
    byte colPins[COLS] = {25, 24, 23, 22};

    Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
    
    void setup() 
    {
      Serial.begin(9600);

      pinMode(RED, OUTPUT);
      pinMode(GREEN, OUTPUT);
      pinMode(BLUE, OUTPUT);

      pinMode(CLOCK,OUTPUT);
      pinMode(LATCH,OUTPUT);
      pinMode(DATA,OUTPUT);

      pinMode(DIGIT_1,OUTPUT);
      pinMode(DIGIT_2,OUTPUT);
      pinMode(DIGIT_3,OUTPUT);
      pinMode(DIGIT_4,OUTPUT);

      digitalWrite(DIGIT_1,LOW);
      digitalWrite(DIGIT_2,LOW);
      digitalWrite(DIGIT_3,LOW);
      digitalWrite(DIGIT_4,LOW);

      for(int i = 22; i < 30; i++)
      {
        pinMode(i, INPUT);
      }
      
      SetColour('r');
      delay(1000);
      digitalWrite(RED, LOW);
      
      for(auto i: password) digits_in_key++;
      
    }

    int Delay = 1;
    int value;

    void loop() 
    {
      char pressed_key = keypad.getKey();
      DisplayAttempt();  // Continuously refresh the display
      if(pressed_key) HandleInput(pressed_key);
    }



    void HandleInput(char key)
    {
      switch(key)
      {
        default:

          attempt[current_digit] = key;
          current_digit++;
          Serial.println(attempt);
          break;
        case '#':
          SubmitAttempt();
          break;
        case '*':
          ResetPassword();
          break;
      }

      if(current_digit == digits_in_key)
      {
        SubmitAttempt();
      }
    }

    void DisplayAttempt() 
    {
      if (current_digit == 0)  
      {
        SetDigit(0);
        return;
      }

      for (int i = 0; i < current_digit; i++) 
      {
        Display(attempt[i] - '0');  // Convert char to int
        SetDigit(i + 1);
        delay(1);  // Hold each digit longer so all are visible
      }
    }

    void SubmitAttempt()
    {
      DisplayAttempt();  // Ensure full input is shown before resetting

      if(strcmp(attempt, password) == 0)
      {
        SetColour('g');
        delay(2000);
        SetColour('0');
      }
      else
      {
        SetColour('r');
        delay(200);
        SetColour('0');
        delay(200);
        SetColour('r');
        delay(200);
        SetColour('0');
        delay(100);
      }
      DisplayAttempt();  // Ensure full input is shown before resetting
      ResetPassword();
      ClearDisplay();  // Clear display after delay
    }

    void ResetPassword()
    {
      Serial.println("Reset Password");
      attempt[5] = {};
      current_digit = 0;
    }

    void ClearDisplay()
    {
      SetDigit(0);
      Display(16);  // Show blank display (16th index in `table[]` is blank)
    }

    void SetColour(char colour)
    {
      switch(colour)
      {
        case 'r':
          digitalWrite(GREEN, LOW);
          digitalWrite(BLUE, LOW);
          digitalWrite(RED, HIGH);
          break;
        case 'g':
          digitalWrite(GREEN, HIGH);
          digitalWrite(BLUE, LOW);
          digitalWrite(RED, LOW);
          break;
        case 'b':
          digitalWrite(GREEN, LOW);
          digitalWrite(BLUE, HIGH);
          digitalWrite(RED, LOW);
          break;
        case '0':
          digitalWrite(GREEN, LOW);
          digitalWrite(BLUE, LOW);
          digitalWrite(RED, LOW);
          break;
      }
    }

    void Display(unsigned char num)
    {
      digitalWrite(LATCH,LOW);
      shiftOut(DATA,CLOCK,MSBFIRST,table[num]);
      digitalWrite(LATCH,HIGH);
      
    }

    void SetDigit(int digit)
    {
      switch(digit)
      {
        case 0:
          digitalWrite(DIGIT_1,LOW);
          digitalWrite(DIGIT_2,LOW);
          digitalWrite(DIGIT_3,LOW);
          digitalWrite(DIGIT_4,LOW);
          break;
        case 1:
          digitalWrite(DIGIT_1,HIGH);
          digitalWrite(DIGIT_2,LOW);
          digitalWrite(DIGIT_3,LOW);
          digitalWrite(DIGIT_4,LOW);
          break;
        case 2:
          digitalWrite(DIGIT_1,LOW);
          digitalWrite(DIGIT_2,HIGH);
          digitalWrite(DIGIT_3,LOW);
          digitalWrite(DIGIT_4,LOW);
          break;
        case 3:
          digitalWrite(DIGIT_1,LOW);
          digitalWrite(DIGIT_2,LOW);
          digitalWrite(DIGIT_3,HIGH);
          digitalWrite(DIGIT_4,LOW);
          break;
        case 4:
          digitalWrite(DIGIT_1,LOW);
          digitalWrite(DIGIT_2,LOW);
          digitalWrite(DIGIT_3,LOW);
          digitalWrite(DIGIT_4,HIGH);
          break;
      }
    }
