#include <Q2HX711.h>

/*---------- Global variables ----------*/
const byte hx711_data_pin = 2;
const byte hx711_clock_pin = 3;
float y1 = 505.0; // calibrated mass to be added
long x1 = 0L;
long x0 = 0L;
float avg_size = 10.0; // amount of averages for each mass measurement

/*---------- Prototype of Functions ----------*/
void Set_Tare();
void Calibration();
float mass(long X0, long X1, float Y1);
void PrintMass();

/*---------- HX711 Parameters ----------*/
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

void setup() {
  Serial.begin(9600); // prepare serial port
  delay(1000);  // allow load cell and hx711 to settle
  Set_Tare(); // tare procedure
  Calibration(); // calibration procedure (mass should be added equal to y1)
  PrintMass(); // prints the mass values
}

void loop() {

}

/*---------- Development of Functions ----------*/
void Set_Tare(){
   for (int i=0;i<int(avg_size);i++){
    delay(10);
    x0+=hx711.read();
   }
  //Serial.println("x0 = ");
  //Serial.println(x0);
  x0/=long(avg_size);
  Serial.println("Add a known Mass");
}

void Calibration(){
  int i = 1;
  while(true){
    if (hx711.read()<x0+1000){
      //Serial.println("Hx read: ");
      //Serial.println(hx711.read());
      Serial.print("-");
      delay(1000);
    } else {
      i++;
      delay(2000);
      for (int j=0;j<int(avg_size);j++){
        x1+=hx711.read();
      }
      x1/=long(avg_size); 
      break;
    }
  }
  Serial.println("\nCalibration Complete!");
  //Serial.println("x1: ");
  //Serial.println(x1);
}

float mass(long X0, long X1, float Y1){
  long reading = 0;
    for (int jj=0;jj<int(avg_size);jj++){
      reading+=hx711.read();
    }
  reading/=long(avg_size); // averaging reading
  // calculating mass based on calibration and linear fit
  float ratio_1 = (float) (reading-x0);
  float ratio_2 = (float) (x1-x0);
  float ratio = ratio_1/ratio_2;
  float weight = (y1*ratio)/1000.0;
  return weight;
}

void PrintMass(){
  int i = 1;
  float weight = 0.0;
  float weightLess = 0.001;
  float weightPlus = 0.001;
  while (true) {
    weight = mass(x0,x1,y1);
    if ( (weight>(mass(x0,x1,y1)-weightLess))&&(weight<(mass(x0,x1,y1)+weightPlus)) ) {
     
    } else {
      weight = mass(x0,x1,y1);
      if (weight <= 0.013) {
        weight = 0.000;
      }
       Serial.print(weight,3);
       Serial.println(" kg");
       i++;
    }
  }
}
