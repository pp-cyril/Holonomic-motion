
#include <PS4BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
PS4BT PS4(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
//PS4BT PS4(&Btd);

bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;

// motion matrix //
float a11 = -0.585, a12 = 0, a13 = 0.4142;
float a21 = 0.2928, a22 = -0.7071, a23 = 0.2928;
float a31 = 0.2928, a32 = 0.7071, a33 = 0.2928;


double x, y, w = 0;
double s1, s2, s3;
double lx, ly, rx, ry;

int d1 = 29, d2 = 31, d3 = 35; //motor direction pins//
int m1 = 7, m2 = 8, m3 = 9; //motor pwm pins//

float sf = 1; // scale factor



void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
}
void loop()
{
  Usb.Task();
  PS4.setLed(Red);

  if (PS4.connected())
  {
    if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117 || PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117 || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) < 117 || PS4.getAnalogHat(RightHatY) > 137 || PS4.getAnalogHat(RightHatY) < 117)
    {
      x = PS4.getAnalogHat(LeftHatX);
      y = PS4.getAnalogHat(LeftHatY);
      w = PS4.getAnalogHat(RightHatX);

      x = map(x, 0, 255, -255, 255);
      y = -map(y, 0, 255, -255, 255);
      w = map(w, 0, 255, 100, -100);

      Serial.print("x= ");
      Serial.print(x);
      Serial.print("  y= ");
      Serial.print(y);
      Serial.print("  w= ");
      Serial.println(w);


      s1 = -sf * ((a11 * y) + (a12 * x) + (a13 * w));
      s2 = -sf * ((a21 * y) + (a22 * x) + (a23 * w));
      s3 = -sf * ((a31 * y) + (a32 * x) + (a33 * w));


      digitalWrite(d1, s1 >= 0 ? 1 : 0);
      digitalWrite(d2, s2 >= 0 ? 1 : 0);
      digitalWrite(d3, s3 >= 0 ? 1 : 0);

      analogWrite(m1, abs(s1));
      analogWrite(m2, abs(s2));
      analogWrite(m3, abs(s3));

    }
    else
    {

      analogWrite(m1, 0);
      analogWrite(m2, 0);
      analogWrite(m3, 0);
    }

  }
  if (PS4.getButtonClick(UP))
  {
    sf += 0.5;
  }

  if (PS4.getButtonClick(DOWN))
  {
    sf -= 0.5;
  }

}


}
