#define NUM_LEDS 40
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include "IRremoteESP8266.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "IRrecv.h"
#include "IRutils.h"
#define PIN 5

#define RED 0
#define GREEN 1
#define BLUE 2

WiFiUDP Udp;

uint16_t local_udp_port = 1111;
const char* ssid     = "Ayanami Rei";
const char* password = "20032203";

decode_results results;
IRrecv irrecv(2); //Указываем вывод, к которому подключен приемник

uint8_t SpeedLight = 20;
int mode = 0;
int speed = 0;
uint8_t color[3] = {255, 0, 0};
int MeteorSpeed = 30;


int colors;
int colors1 = 255;
int colors2;

byte brightless = 50;
CRGB leds[NUM_LEDS];
byte counter;
uint32_t last_cmd = 0;




void setup() {
  Serial.begin(9600); // выставляем скорость COM порта
  irrecv.enableIRIn();

  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  pinMode(13, OUTPUT);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Udp.begin(local_udp_port);
}


void ir_recv()
{
  if (irrecv.decode( &results ))
  {
    uint32_t cmd = results.value;
    if (cmd == 0xffffffff )
    {
      cmd = last_cmd;
    }
    // если данные пришли
    switch ( cmd )
    {
      case 0xFF18E7:
        brightless += 5;
        break;
      case 0xFF4AB5:
        brightless = brightless - 5;
        break;
      case 0xffa25d: mode = 0; break;     //Кнопка 1
      case 0xff629d: mode = 1; break;     //Кнопка 2
      case 0xffe21d: mode = 2; break;     //Кнопка 3
      case 0xFF22DD: mode = 3; break;     //Кнопка 4
      case 0xFF02FD: mode = 4; break;     //Кнопка 5
      case 0xFFC23D: mode = 5; break;     //Кнопка 6
      case 0xFFE01F: mode = 6; break;     //Кнопка 7
      case 0xFFA857: mode = 7; break;     //Кнопка 8
      case 0xFF906F: mode = 8; break;     //Кнопка 9
      case 0xFF10EF:
        SpeedLight += 1;
        MeteorSpeed ++;
        if (MeteorSpeed > 70)
        {
          MeteorSpeed = 70;
        };
        ///
        if (SpeedLight > 80)
        {
          SpeedLight = 80;
        };
        break;
      case 0xFF5AA5:
        SpeedLight -= 1;
        MeteorSpeed --;

        if (MeteorSpeed < 30)
        {
          MeteorSpeed = 30;
        };
        ///
        if (SpeedLight < 15)
        {
          SpeedLight = 15;
        };
        break;


      case 0xFF38C7:                      //Кнопка 0
 
        { 
          color[GREEN] = 255;
          color[RED] = 255;
          color[BLUE] = 255;
        }; break;
        
      case 0xFF9867:                      //Кнопка OK
        {
          color[GREEN] = colors;
          color[RED] = colors1;
          color[BLUE] = colors2;
        }; break;

      case 0xFF6897:                      //Кнопка *
        {
        if(color[GREEN] != 255, color[RED] != 255, color[BLUE] != 255)
        {
          colors = color[GREEN];
          colors1 = color[RED];
          colors2 = color[BLUE];
        };

          //Коли R = 255 G ++;   Коли G = 255 R --;   Коли R = 0 B ++;  Коли B = 255 G --;  Коли G = 0 R ++;  Коли R = 255 B --;
          if ((color[RED] > 250) && (color[BLUE] < 15))
          {
            color[GREEN] += 15;
          }

          if ((color[GREEN] > 250) && (color[BLUE] < 15))
          {
            color[RED] -= 15;
          }

          if ((color[GREEN] > 250) && (color[RED] < 15))
          {
            color[BLUE] += 15;
          }


          if ((color[BLUE] > 250) && (color[RED] < 15))
          {
            color[GREEN] -= 15;
          }

          if ((color[GREEN] < 15) && (color[BLUE] > 250))
          {
            color[RED] += 15;
          }

          if ((color[RED] > 250) && (color[GREEN] < 15 ))
          {
            color[BLUE] -= 15;
          }

        }; break;

      case 0xFFB04F:                      //Кнопка #
        {
          
        if(color[GREEN] != 255, color[RED] != 255, color[BLUE] != 255)
        {
          colors = color[GREEN];
          colors1 = color[RED];
          colors2 = color[BLUE];
        };
          
          if ((color[RED] > 250) && (color[GREEN] < 15))
          {
            color[BLUE] += 15;
          }

          if ((color[BLUE] > 250) && (color[GREEN] < 15))
          {
            color[RED] -= 15;
          }

          if ((color[BLUE] > 250) && (color[RED] < 15))
          {
            color[GREEN] += 15;
          }

          if ((color[GREEN] > 250) && (color[RED] < 15))
          {
            color[BLUE] -= 15;
          }

          if ((color[BLUE] < 15) && (color[GREEN] > 250))
          {
            color[RED] += 15;
          }

          if ((color[RED] > 250) && (color[BLUE] < 15 ))
          {
            color[GREEN] -= 15;
          }

        }; break;

    }
    last_cmd = cmd;

    if (brightless > 250)
    {
      brightless = 250;
    }
    if (brightless < 5)
    {
      brightless = 5;
    }
    irrecv.resume();
    Serial.print(color[RED], HEX);
    Serial.print(color[GREEN], HEX);
    Serial.println(color[BLUE], HEX);
    Serial.println(brightless);
    Serial.println(mode);
    Serial.println("SpLight");
    Serial.println(SpeedLight);
    Serial.println("SpMeteor");
    Serial.println(MeteorSpeed);
    yield();
  }
}


void loop()
{

  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    uint8_t incomingPacket = 0;
    
    
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(&incomingPacket, 1);
    if (len > 0)
    {
      if(incomingPacket >= '0')
      {
      mode = incomingPacket-'0';
      }
    }
   
  }
  

  FastLED.setBrightness(brightless);
  switch (mode)
  {
    case 0:                                                           //РЕЖИМ №1
      {
        for (int i = 0; i < NUM_LEDS; i++ )
        { // от 0 до первой трети
          leds[i] = CHSV(counter + i * 2, 255, 255);  // HSV. Увеличивать HUE (цвет)
          // умножение i уменьшает шаг радуги
        }
        counter++;        // counter меняется от 0 до 255 (тип данных byte)
        delay(SpeedLight);
      }; break;

    case 1:                                                           //РЕЖИМ №2
      {
        for (int i = 0; i < NUM_LEDS; i++ )
        { // от 0 до первой трети
          //leds[i] = CRGB( color[RED], color[GREEN], color[BLUE]);
          leds[i].setRGB(  color[RED],  color[BLUE], color[GREEN]);
        }
      }; break;

    case 2:                                                           //РЕЖИМ №3
      {
        RGBLoop();
      }; break;

    case 3:                                                           //РЕЖИМ №4
      {
        CylonBounce(color[RED], color[BLUE],  color[GREEN], 4, 4, 50);
      }; break;

    case 4:                                                           //РЕЖИМ №5
      {
        NewKITT(color[RED], color[BLUE],  color[GREEN], 4, SpeedLight, 50);
      }; break;

    case 5:                                                           //РЕЖИМ №6
      {
        byte colors[3][3] = {
          {0xff, 0, 0},
          {0xff, 0xff, 0xff},
          {0   , 0   , 0xff}
        };
        BouncingColoredBalls(3, colors);
      }; break;

    case 6:                                                           //РЕЖИМ №7
      {
        Fire(55, 120, 15);
      }; break;

    case 7:                                                           //РЕЖИМ №8
      {
        meteorRain(color[RED], color[BLUE],  color[GREEN], 10, 64, true, MeteorSpeed);
      }; break;
      
    case 8:                                                           //РЕЖИМ №9
      {
        RunningLights(color[RED], color[BLUE], color[GREEN], 50);
      }; break;

    default: mode = 0; break;
  }

  FastLED.show();
  ir_recv();
  yield();
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
      yield();
      delay(SpeedDelay);
      if (mode != 3)
      {
        return;
      }
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    ir_recv();
    yield();
    delay(SpeedDelay);
    if (mode != 3)
    {
      return;
    }
  }

  delay(SpeedDelay);

  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
      yield();
      delay(SpeedDelay);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    ir_recv();
    yield();
    delay(SpeedDelay);
  }
}

//////////////////////////////////////////////////////

void RGBLoop() {
  for (int j = 0; j < 3; j++ ) {
    // Fade IN
    for (int k = 0; k < 256; k++) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }

      if (mode != 2)
      {
        return;
      }

      showStrip();
      ir_recv();
      yield();
    }
    // Fade OUT
    for (int k = 255; k >= 0; k--) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }

      if (mode != 2)
      {
        return;
      }

      showStrip();
      ir_recv();
      yield();
    }
  }
}

//////////////////////////////////////////////////////

void BouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;

  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
    ir_recv();
    yield();
  }

  while (mode == 5)
  {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

      if ( Height[i] < 0 ) {
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();

        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
        ir_recv();
        yield();
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
      ir_recv();
      yield();
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
      ir_recv();
      yield();
    }

    showStrip();
    setAll(0, 0, 0);
    ir_recv();
    yield();
  }
  ir_recv();
  yield();
}

//////////////////////////////////////////////////////

void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  
  if (mode != 4)
  {
    return;
  }
  
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);

  
  if (mode != 4)
  {
    return;
  }
  
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);

  if (mode != 4)
  {
    return;
  }
  
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  
  if (mode != 4)
  {
    return;
  }
  
  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  
  if (mode != 4)
  {
    return;
  }
  
  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  if (mode != 4)
  {
    return;
  }
  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  if (mode != 4)
  {
    return;
  }
  
  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  if (mode != 4)
  {
    return;
  }
}

void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    ir_recv();
    yield();
    delay(SpeedDelay);
  }
  ir_recv();
  yield();
  delay(ReturnDelay);
}

void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) 
{
  for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++) {
    setAll(0, 0, 0);

    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS - i - j, red, green, blue);
    }
    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

    showStrip();
    ir_recv();
    yield();
    delay(SpeedDelay);
  }
  ir_recv();
  yield();
  delay(ReturnDelay);

}

void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    ir_recv();
    yield();
    delay(SpeedDelay);
  }
  ir_recv();
  yield();
  delay(ReturnDelay);
}

void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    ir_recv();
    yield();
    delay(SpeedDelay);
  }
  ir_recv();
  yield();
  delay(ReturnDelay);
}

//////////////////////////////////////////////////////

void showStrip() {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  FastLED.show();
#endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
#endif
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
    yield();
  }
  showStrip();
}

//////////////////////////////////////////////////////

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
  yield();
}

//////////////////////////////////////////////////////

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {


    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
      ir_recv();

      if (mode != 7)
      {
        return;
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
        setPixel(i - j, red, green, blue);
      }
      ir_recv();
      if (mode != 7)
      {
        return;
      }
    }

    showStrip();
    delay(SpeedDelay);
  }

}

void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy( fadeValue );
#endif
}

//////////////////////////////////////////////////////

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
      if (mode != 8)
      {
        return;
      }
      ir_recv();
      yield();
  int Position=0;
  
  for(int j=0; j<NUM_LEDS*2; j++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      if (mode != 8)
      {
        return;
      }
      ir_recv();
      yield();
      }
      
      showStrip();
      delay(WaveDelay);
      ir_recv();
      yield();
  }
}
