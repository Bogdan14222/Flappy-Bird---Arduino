#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


#define OLED_RESET    -1  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define BUTTON_PIN 35    
#define BUZZER_PIN 32  


#define SPRITE_WIDTH  16
#define SPRITE_HEIGHT 16


static const unsigned char PROGMEM wing_down_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x1F, 0xF0, 
  0x3F, 0x1C, 0x7F, 0xFE, 0xFF, 0x81, 0xDF, 0x7E, 
  0xDF, 0x78, 0xDF, 0x78, 0xCE, 0x78, 0xF1, 0xF0, 
  0x7F, 0xE0, 0x3F, 0xC0, 0x07, 0x00, 0x00, 0x00
};
static const unsigned char PROGMEM wing_up_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x1F, 0xF0, 
  0x3F, 0x1C, 0x71, 0xFE, 0xEE, 0xC1, 0xDF, 0x7E, 
  0xDF, 0x78, 0xFF, 0xF8, 0xFF, 0xF8, 0xFF, 0xF0, 
  0x7F, 0xE0, 0x3F, 0xC0, 0x07, 0x00, 0x00, 0x00
};


int score = 0;              
int bird_x = SCREEN_WIDTH / 4;  
int bird_y = 0;              
int velocity = 0;            
bool gameRunning = false;    
bool firstGame = true;       


int wall_x[2];              
int wall_y[2];              
const int wall_width = 10;  
const int wall_gap   = 30;   
const int wall_speed = 4;    


void startGame() {
 
  bird_y = SCREEN_HEIGHT / 2 - SPRITE_HEIGHT / 2;
  velocity = 0;
  score = 0;

  wall_x[0] = SCREEN_WIDTH;
  wall_y[0] = random(0, SCREEN_HEIGHT - wall_gap);
  wall_x[1] = SCREEN_WIDTH + (SCREEN_WIDTH / 2);
  wall_y[1] = random(0, SCREEN_HEIGHT - wall_gap);

  gameRunning = true;
  firstGame = false;
}

void setup() {
  
  Serial.begin(115200);


  Wire.begin(26, 27);

  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    
    Serial.println("Eroare la initializarea display-ului OLED");
    while (true);
  }
  display.clearDisplay();
  display.display();



  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);     


  randomSeed(analogRead(34));
}

void loop() {
 
  if (gameRunning) {
  

   
    display.clearDisplay();

   
    if (digitalRead(BUTTON_PIN) == LOW) {
      velocity = -4;
    }

    
    velocity += 1;
    
    bird_y += velocity;

  
    if (bird_y < 0) {
      bird_y = 0;
      velocity = 0;  
    }
    
    if (bird_y > SCREEN_HEIGHT - SPRITE_HEIGHT) {
      bird_y = SCREEN_HEIGHT - SPRITE_HEIGHT;
     
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      gameRunning = false;           
      
    }

   
    if (velocity < 0) {
      
      if (random(2) == 0) {
        display.drawBitmap(bird_x, bird_y, wing_down_bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SSD1306_WHITE);
      } else {
        display.drawBitmap(bird_x, bird_y, wing_up_bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SSD1306_WHITE);
      }
    } else {
     
      display.drawBitmap(bird_x, bird_y, wing_up_bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SSD1306_WHITE);
    }


    for (int i = 0; i < 2; i++) {
    
      display.fillRect(wall_x[i], 0, wall_width, wall_y[i], SSD1306_WHITE);
   
      display.fillRect(wall_x[i], wall_y[i] + wall_gap, wall_width, SCREEN_HEIGHT - (wall_y[i] + wall_gap), SSD1306_WHITE);


      if (wall_x[i] + wall_width < 0) {
        wall_x[i] = SCREEN_WIDTH;
        wall_y[i] = random(0, SCREEN_HEIGHT - wall_gap);
      }


      if (wall_x[i] + wall_width == bird_x) {
        score++;

        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
      }


      if (
        (bird_x + SPRITE_WIDTH > wall_x[i] && bird_x < wall_x[i] + wall_width) && 
        (bird_y < wall_y[i] || bird_y + SPRITE_HEIGHT > wall_y[i] + wall_gap)     
      ) {
        
        display.display(); 
        digitalWrite(BUZZER_PIN, HIGH);
        delay(500);                   
        digitalWrite(BUZZER_PIN, LOW);
        gameRunning = false;

        break;
      }

      wall_x[i] -= wall_speed;
    }


    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Score: ");
    display.print(score);


    display.display();

    delay(50);


  } else {


    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    if (firstGame) {

      display.setTextSize(2);

      const char* title = "FLAPPY BIRD";
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 10);
      display.print(title);

      display.setTextSize(1);
      display.setCursor(18, 40);
      display.print("Press button to START");
    } else {

      display.setTextSize(2);

      const char* gm = "GAME OVER";
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(gm, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 5);
      display.print(gm);

      display.setTextSize(1);
      char scoreMsg[16];
      sprintf(scoreMsg, "Score: %d", score);
      display.setCursor((SCREEN_WIDTH - 6 * strlen(scoreMsg)) / 2, 30);
      display.print(scoreMsg);

      display.setCursor(12, 50);
      display.print("Press button to restart");
    }

    display.display();


    while (digitalRead(BUTTON_PIN) == LOW) {

    }

    while (digitalRead(BUTTON_PIN) == HIGH) {
      Serial.println(digitalRead(BUTTON_PIN));
      if(digitalRead(BUTTON_PIN) == LOW)
        break;
    }

    
    startGame();
   
  }
}

