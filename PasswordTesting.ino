    #include <Keypad.h>

    #define RED 50
    #define GREEN 51
    #define BLUE 52

    const byte ROWS = 4;
    const byte COLS = 4;

    const char password[4] = "1234";
    char attempt[4] = "";
    int current_digit = 0;     
    int digits_in_key;

    char hexaKeys[ROWS][COLS] = {
      {'1','2','3','A'},
      {'4','5','6','B'},
      {'7','8','9','C'},
      {'*','0','#','D'}
    };
    
    byte rowPins[ROWS] = {2, 3, 4, 5};
    byte colPins[COLS] = {6, 7, 8, 9};

    Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
    
    void setup() 
    {
      Serial.begin(9600);

      pinMode(RED, OUTPUT);
      pinMode(GREEN, OUTPUT);
      pinMode(BLUE, OUTPUT);

      for(int i = 2; i < 9; i++)
      {
        pinMode(i, INPUT);
      }
      
      SetColour('r');
      delay(1000);
      digitalWrite(RED, LOW);
      
      for(auto i: password) digits_in_key++;
      
    }

    void loop() 
    {
      char pressed_key = keypad.getKey();
      
      /*if(pressed_key && pressed_key != '#' && pressed_key != '*')
      {
        SetColour('b');
        Serial.println("0-9 pressed");
      }
      else if(pressed_key == '#')
      {
        SetColour('g');
        Serial.println("# pressed");

      }
      else if(pressed_key == '*')
      {
        SetColour('r');
        Serial.println("* pressed");
      }*/
    }

    void SubmitAttempt()
    {
        SetColour('g');
    }

    void ResetPassword()
    {
      Serial.println("Reset Password");
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
