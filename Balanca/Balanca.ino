#include <Q2HX711.h>
#include <LiquidCrystal.h> //include the display library

/*---------- Global variables ----------*/
const byte hx711_data_pin = 2;
const byte hx711_clock_pin = 3;
float y1 = 141.0; // calibrated mass to be added
long x1 = 0L;
long x0 = 0L;
float avg_size = 100.0; // amount of averages for each mass measurement
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // initialize the library by associating LCD interface pin
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // with the arduino pin number it is connected to

/*---------- Prototype of Functions ----------*/
void Set_Tare();
void Calibration();
float mass(long X0, long X1, float Y1);
void PrintMass(float w);
void PowerDown();

/*---------- HX711 Parameters ----------*/
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

void setup() {
  Serial.begin(9600); // prepare serial port
  delay(1000);  // allow load cell and hx711 to settle
  lcd.begin(16, 2); // set up the LCD's number of columns and rows
  lcd.clear(); // cleans the display
  Set_Tare(); // tare procedure
  Calibration(); // calibration procedure (mass should be added equal to y1)
}

void loop() {
  int i = 1;
  int x = 0;
  float weight = 0.0;
  float PreviousWeight = 0.0; 
  lcd.clear();
  while (true) {
    weight = mass(x0,x1,y1);
    if ( (weight>=(PreviousWeight-0.005)) && (weight<=(PreviousWeight+0.005)) ) { // waits until mass is changed +/-5g
      x++;
      //x1 -= 0000100;
      if (x >= 10){
        PowerDown();
        x = 0;
      }
    } else {
       //if (weight<=0.000){
        //weight = 0.000;
        //weight = mass(x0,x1,y1);
        //x1 += 0000100;
       //}
       x = 0;
       PrintMass(weight);
       PreviousWeight = weight;
       i++;
      }
   }
}

/*---------- Development of Functions ----------*/
void Set_Tare(){
  lcd.print("Hello");
   for (int i=0;i<int(avg_size);i++){
    delay(10);
    x0+=hx711.read();
   }
  x0/=long(avg_size);
}

void Calibration(){
  int i = 1;
  lcd.clear();
  while(true){
    if (hx711.read()<x0+1000){
      lcd.setCursor(0,0);
      lcd.print("Add a known Mass");
      lcd.setCursor(0,1);
      lcd.print("...");
      lcd.blink();
    } else {
      i++;
      delay(2000);
      lcd.clear();
      for (int j=0;j<int(avg_size);j++){
        x1+=hx711.read();
        lcd.setCursor(0,0);
        lcd.print("Calibrating ***");
      }
      x1/=long(avg_size); 
      break;
    }
  }
  lcd.noBlink();
  lcd.clear();
  lcd.print("Calibration OK!");
}

float mass(long X0, long X1, float Y1){
  long reading = 0;
    for (int jj=0;jj<12;jj++){
      reading+=hx711.read();
    }
  reading/=12; // averaging reading
  // calculating mass based on calibration and linear fit
  float ratio_1 = (float) (reading-x0);
  float ratio_2 = (float) (x1-x0);
  float ratio = ratio_1/ratio_2;
  float weight = (y1*ratio)/1000.0;
  return weight;
}

void PrintMass(float w){
  lcd.setCursor(0,0);
  lcd.print(w,3);
  lcd.print(" kg");
  lcd.setCursor(0,1);
  lcd.print(x1);
}

void PowerDown(){
   digitalWrite(3, HIGH); // turn the SCK pin on HIGH
   lcd.clear();
   lcd.print("Sleeping");
   delay(5000);
   digitalWrite(3, LOW);
   //x1 += 0000010;
}
