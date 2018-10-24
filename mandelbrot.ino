// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <FixedPoints.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// TOUCH PINS
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// calibration mins and max for raw data when touching edges of screen
#define TS_MINX 210
#define TS_MINY 210
#define TS_MAXX 915
#define TS_MAXY 910

#define MINPRESSURE 1
#define MAXPRESSURE 1000

//LED PINS
#define RLED 11
#define GLED 12
#define BLED 10

#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x07FF
#define LIGHTGREY 0xDEDB
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364);

//Container variables for touch coordinates
int X, Y, Z;
int RGB = 0; //color mode selector: 0 = R; 1 = G; 2 = B;

short max_color = ((28 & 0xF8) << 8) | ((13 & 0xFC) << 3) | (10 >> 3);

int num_rows = tft.width();
int num_cols = tft.height();

float min_x, max_x, min_y, max_y;
int zoom;
int started = 0;

void default_vals(void){
  min_x = -2.5;
  max_x = 1.0;
  min_y = -1.0;
  max_y = 1.0;
  zoom = 1;
}

void setup(void) {
  Serial.begin(9600);
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  default_vals();
  digitalWrite(RLED, HIGH);
  digitalWrite(GLED, LOW);
  digitalWrite(BLED, LOW);
  draw_menu();
}

void loop(void) {
  //wait for a touch to find a new zoom point
  while(1)
  {
    retrieveTouch();
    if (Z > MINPRESSURE && Z < MAXPRESSURE) {
      if(X > 50){
        if(started == 0){
          break;
        }
        else{
          set_zoom(X, Y);
          Serial.print("ZOOM POINT: ");
          Serial.print(X);
          Serial.print(", ");
          Serial.println(Y);       
          break;
        }
      }
      else{
        if(Y < 80) {
          //blue
          RGB = 2;
          digitalWrite(RLED, LOW);
          digitalWrite(GLED, LOW);
          digitalWrite(BLED, HIGH);
        }
        else if(Y < 160){
          //green
          RGB = 1;
          digitalWrite(RLED, LOW);
          digitalWrite(GLED, HIGH);
          digitalWrite(BLED, LOW);
        }
        else{
          //red
          RGB = 0;
          digitalWrite(RLED, HIGH);
          digitalWrite(GLED, LOW);
          digitalWrite(BLED, LOW);
        }
      }
    }
  }
  Serial.println(mandelbrot());
}


void draw_menu(void)
{
    tft.fillScreen(max_color); // black-ish screen
    tft.drawFastHLine(0, 50, 240, 0xFFFF);
    tft.drawFastVLine(80, 0, 50, 0xFFFF);
    tft.drawFastVLine(160, 0, 50, 0xFFFF);
    tft.setRotation(1);
    tft.setTextSize(5);
    tft.setCursor(13, 22);
    tft.setTextColor(RED);
    tft.println("R");
    tft.setCursor(13, 102);
    tft.setTextColor(GREEN);
    tft.println("G");
    tft.setCursor(13, 182);
    tft.setTextColor(BLUE);
    tft.println("B");
    tft.setTextSize(3);
    tft.setCursor(65, 95);
    tft.setTextColor(WHITE);
    tft.println("MANDELBROTTER");
    tft.setTextSize(2);
    tft.setCursor(68, 118);
    tft.println("ISEC TEST PROJECT");
    
    tft.setRotation(0);
}

unsigned long mandelbrot(void){
  unsigned long counter = micros();
  started = 1;
  int i, j;
  float x, y;
  float x0, y0;
  float xtemp, ytemp;
  int iteration;
  int max_iteration = 15;

  //draw a mandelbrot
  for (i = 0; i < num_rows; i++) {
    for (j = 51; j < num_cols; j++) {
      x = 0;
      y = 0;
      //scaled x and y coordinates
      x0 = j/320.0*(max_x - min_x) + min_x;
      y0 = (239.0-i)/240.0*(max_y - min_y) + min_y;
      iteration = 0;

      // using periodicity shortcut
      while (x*x + y*y < 4  &&  iteration < max_iteration) {
        xtemp = x*x - y*y + x0;
        ytemp = 2*x*y + y0;
        if (x == xtemp  &&  y == ytemp) {
          iteration = max_iteration;
        break;
        }
        x = xtemp;
        y = ytemp88[;
        iteration++;/
      }
      //for black
      if(iteration == max_iteration) {
        tft.drawPixel(i, j, max_color);
      }       
      else {
        if(RGB == 0){
          //               r                             g                              b
          short color = ((((iteration*18)/zoom) & 0xF8) << 8) | ((((iteration)/zoom) & 0xFC) << 3) | (((iteration)/zoom) >> 3);
          tft.drawPixel(i, j, color);
        }
        else if(RGB == 1){
          short color = ((((iteration)/zoom) & 0xF8) << 8) | ((((iteration*18)/zoom) & 0xFC) << 3) | (((iteration)/zoom) >> 3);
          tft.drawPixel(i, j, color);
        }
        else if(RGB == 2){
          short color = ((((iteration)/zoom) & 0xF8) << 8) | ((((iteration)/zoom) & 0xFC) << 3) | (((iteration*18)/zoom) >> 3);
          tft.drawPixel(i, j, color);
        }
      }
    }
  }
  return micros() - counter;
}

void retrieveTouch()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  //If sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  //default is vertical - had to swap and finagle the axes for horizontal
  Y = tft.width() - map(p.x, TS_MAXX, TS_MINX, 0, tft.width());
  X = tft.height() - map(p.y, TS_MAXY, TS_MINY, 0, tft.height());
  Z = p.z;
}

int set_zoom(int target_x, int target_y)
{
  float x0 = target_x/320.0*(max_x - min_x) + min_x;
  float y0 = (239.0-target_y)/240.0*(max_y - min_y) + min_y;
  Serial.print("X0 and Y0: ");
  Serial.print(x0);
  Serial.print(", ");
  Serial.println(y0);
  min_x = x0 - (1/(pow(1.5, zoom)));
  max_x = x0 + (1/(pow(1.5, zoom)));
  min_y = y0 - (0.75/(pow(1.5, zoom)));
  max_y = y0 + (0.75/(pow(1.5, zoom)));
  zoom += 1;
  tft.fillRect(0,51,240,269,max_color);
}
