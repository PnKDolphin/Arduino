  #include <Keypad.h>
  #include <string.h>

    #define RED 50
    #define GREEN 51
    #define BLUE 52

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

    void SubmitAttempt()
    {
      if(strcmp(attempt, password) == 0)
      {
        SetColour('g');
        attempt[5] = {};
        current_digit = 0;
        delay(2000);
        SetColour('0');
      }
      else
      {
        SetColour('r');
        attempt[5] = {};
        current_digit = 0;
        delay(200);
        SetColour('0');
        delay(200);
        SetColour('r');
        delay(200);
        SetColour('0');
      }
    }

    void ResetPassword()
    {
      Serial.println("Reset Password");
      attempt[5] = {};
      current_digit = 0;
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
