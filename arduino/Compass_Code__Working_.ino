// There is no tilt compensation, so make sure compass is aligned correctly, and stable!

#include <Wire.h>

#define HMC5883L_ADDRESS 0x1E
#define HMC5883L_DEFAULT_ADDRESS 0x1E

//calibrated_values[3] is the global array where the calibrated data will be placed
//calibrated_values[3]: [0]=Xc, [1]=Yc, [2]=Zc
float calibrated_values[3];   
//transformation(float uncalibrated_values[3]) is the function of the magnetometer data correction 
//uncalibrated_values[3] is the array of the non calibrated magnetometer data
//uncalibrated_values[3]: [0]=Xnc, [1]=Ync, [2]=Znc
void transformation(float uncalibrated_values[3])    
{
  //calibration_matrix[3][3] is the transformation matrix
  //replace M11, M12,..,M33 with your transformation matrix data
  double calibration_matrix[3][3] = 
  {
    {1.09, -0.048, -0.014},
    {0.049, 1.053, -0.196},
    {0.036, 0.021, 1.204}  
  };
  //bias[3] is the bias
  //replace Bx, By, Bz with your bias data
  double bias[3] = 
  {
    5.684,
    -59.183,
    -62.012
  };  
  //calculation
  for (int i=0; i<3; ++i) uncalibrated_values[i] = uncalibrated_values[i] - bias[i];
  float result[3] = {0, 0, 0};
  for (int i=0; i<3; ++i)
    for (int j=0; j<3; ++j)
      result[i] += calibration_matrix[i][j] * uncalibrated_values[j];
  for (int i=0; i<3; ++i) calibrated_values[i] = result[i];
}

void setup() {
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(HMC5883L_ADDRESS); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();
}

void loop() {
  int x, y, z; //triple axis data
  float values_from_magnetometer[3]

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(HMC5883L_ADDRESS);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

  //Read data from each axis, 2 registers per axis
  Wire.requestFrom(HMC5883L_ADDRESS, 6);
  if (6 <= Wire.available()) {
    x = Wire.read() << 8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read() << 8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read() << 8; //Y msb
    y |= Wire.read(); //Y lsb
  }

  // Quick Fix from steelgoose
  if (x > 32767)
    x = x - 65536;
  if (y > 32767)
    y = y - 65536;
  if (z > 32767)
    z = z - 65536;

  values_from_magnetometer[0] = x;
  values_from_magnetometer[1] = y;
  values_from_magnetometer[2] = z;
  transformation(values_from_magnetometer);

  Serial.flush();
  Serial.print(calibrated_values[0]);
  Serial.print(",");
  Serial.print(calibrated_values[1]);
  Serial.print(",");
  Serial.print(calibrated_values[2]);
  Serial.println();
  delay(250);
}
