/**
 ******************************************************************************
 * @file    main.cpp
 * @author  CLab
 * @version V1.0.0
 * @date    2-December-2016
 * @brief   Simple Example application for using the X_NUCLEO_IKS01A1 
 *          MEMS Inertial & Environmental Sensor Nucleo expansion board.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
*/ 

/* Includes */
#include "mbed.h"
#include "x_nucleo_iks01a2.h"
#include <string.h>

DigitalOut led0(A0);
DigitalOut led1(A1);
DigitalOut led2(A2);
DigitalOut beep_out(A5);

DigitalOut LCD0(D0);
DigitalOut LCD1(D1);
DigitalOut LCD2(D2);
DigitalOut LCD3(D3);
DigitalOut LCD4(D8);
DigitalOut LCD5(D9);
DigitalOut LCD6(D6);
DigitalOut LCD7(D7);

DigitalOut EN(D10);
DigitalOut RW(D11);
DigitalOut RS(D12);



void beep();
void init_LCD();
void WriteToLCD(int input, int isCmd);

BusOut LCD_output(D7,D6,D9,D8,D3,D2,D1,D0);
//BusOut LCD_output(D0,D1,D2,D3,D4,D5,D6,D7);
int shift_right_counter = 0;

/* Instantiate the expansion board */
static X_NUCLEO_IKS01A2 *mems_expansion_board = X_NUCLEO_IKS01A2::Instance(D14, D15, D4, D5);

/* Retrieve the composing elements of the expansion board */
static LSM303AGR_MAG_Sensor *magnetometer = mems_expansion_board->magnetometer;
static HTS221Sensor *hum_temp = mems_expansion_board->ht_sensor;
static LPS22HBSensor *press_temp = mems_expansion_board->pt_sensor;
static LSM6DSLSensor *acc_gyro = mems_expansion_board->acc_gyro;
static LSM303AGR_ACC_Sensor *accelerometer = mems_expansion_board->accelerometer;

/* Helper function for printing floats & doubles */
static char *printDouble(char* str, double v, int decimalDigits=2)
{
  int i = 1;
  int intPart, fractPart;
  int len;
  char *ptr;

  /* prepare decimal digits multiplicator */
  for (;decimalDigits!=0; i*=10, decimalDigits--);

  /* calculate integer & fractinal parts */
  intPart = (int)v;
  fractPart = (int)((v-(double)(int)v)*i);

  /* fill in integer part */
  sprintf(str, "%i.", intPart);

  /* prepare fill in of fractional part */
  len = strlen(str);
  ptr = &str[len];

  /* fill in leading fractional zeros */
  for (i/=10;i>1; i/=10, ptr++) {
    if(fractPart >= i) break;
    *ptr = '0';
  }

  /* fill in (rest of) fractional part */
  sprintf(ptr, "%i", fractPart);

  return str;
}



/* Simple main function */
int main() {
  uint8_t id;
  float value1, value2;
  char buffer1[32], buffer2[32];
  int32_t axes[3];
  int clearflag = 0;
  //char *mystr = "abcdefghijklmnopqrstuvwxyz!@#$%^.";
      
    //init_LCD();
    
    /*WriteToLCD(0b00111000, 1); // Function Setting 
    wait(100); 
    WriteToLCD(0b00000110, 1); // Entering Mode   
    WriteToLCD(0b00001101, 1); // Display on 
    WriteToLCD(0b00000001, 1); // Clear Screen 
    WriteToLCD(0b10000000, 1); // Move to top left
    
    if(clearflag == 0)
    {
        WriteToLCD(0b00000010, 1); //歸位
        WriteToLCD(0b00000001, 1); //清畫面
        clearflag = 1;
    }
  */

  /* Enable all sensors */
  hum_temp->Enable();
  press_temp->Enable();
  magnetometer->Enable();
  accelerometer->Enable();
  acc_gyro->Enable_X();
  acc_gyro->Enable_G();
  
  printf("\r\n--- Starting new run ---\r\n");

  hum_temp->ReadID(&id);
  printf("HTS221  humidity & temperature    = 0x%X\r\n", id);
  press_temp->ReadID(&id);
  printf("LPS22HB  pressure & temperature   = 0x%X\r\n", id);
  magnetometer->ReadID(&id);
  printf("LSM303AGR magnetometer            = 0x%X\r\n", id);
  accelerometer->ReadID(&id);
  printf("LSM303AGR accelerometer           = 0x%X\r\n", id);
  acc_gyro->ReadID(&id);
  printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);
 
  while(1) {
    printf("\r\n");

    hum_temp->GetTemperature(&value1);
    hum_temp->GetHumidity(&value2);
    printf("HTS221: [temp] %7s C,   [hum] %s%%\r\n", printDouble(buffer1, value1), printDouble(buffer2, value2));
    
    press_temp->GetTemperature(&value1);
    press_temp->GetPressure(&value2);
    printf("LPS22HB: [temp] %7s C, [press] %s mbar\r\n", printDouble(buffer1, value1), printDouble(buffer2, value2));

    printf("---\r\n");

    magnetometer->Get_M_Axes(axes);
    printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
    
    accelerometer->Get_X_Axes(axes);
    printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
    if( axes[0]>=80 || axes[0] <= -80 || axes[1]>= 80 || axes[1] <= -80)
    {
        led0 = !led0;
        led1 = !led1;
        led2 = !led2;
        beep();
        wait(0.2);
    }
    else if( axes[0]>= 50 || axes[0] <= -50 || axes[1]>= 50 || axes[1] <= -50)
    {
        led0 = !led0;
        led1 = !led1;
        wait(0.2);
    }
    else if( axes[0]>= 10 || axes[0] <= -10 || axes[1]>= 10 || axes[1] <= -10)
    {
        led0 = !led0;
        wait(0.2);

    }

    led0 = 0 ;
    led1 = 0 ;
    led2 = 0 ;
    
    acc_gyro->Get_X_Axes(axes);
    printf("LSM6DSL [acc/mg]:      %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

    acc_gyro->Get_G_Axes(axes);
    printf("LSM6DSL [gyro/mdps]:   %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

    wait(1.5);
  }
}


void beep()
{
    for(int i=0;i<10000;i++)
    {
        for(int i=0;i<=2000;i++)
        if(1000>=i)
            beep_out = 1;
        else
            beep_out = 0;
    }
}





/*void WriteToLCD(int input, int isCmd)
{
    if(isCmd == 1)
    {
        RS = 0;
    }
    else
    {
        RS = 1;
    }
    RW = 0 ;
    //LCD_output = input;

    for(int i=0 ; i<8 ; i++ )
    {
        if(input%2 == 0)
        {
            if(i==0)
                LCD0 = 0;
            else if(i==1)
                LCD1 = 0;
            else if(i==2)
                LCD2 = 0;
            else if(i==3)
                LCD3 = 0;
            else if(i==4)
                LCD4 = 0;
            else if(i==5)
                LCD5 = 0;
            else if(i==6)
                LCD6 = 0;
            else if(i==7)
                LCD7 = 0;
        }
        else
        {
            if(i==0)
                LCD0 = 1;
            else if(i==1)
                LCD1 = 1;
            else if(i==2)
                LCD2 = 1;
            else if(i==3)
                LCD3 = 1;
            else if(i==4)
                LCD4 = 1;
            else if(i==5)
                LCD5 = 1;
            else if(i==6)
                LCD6 = 1;
            else if(i==7)
                LCD7 = 1;
        }
      input /= 2;
    }
    EN = 1;
    wait(0.00001);
    EN = 0;
    wait(0.00001);
}*/

/*void init_LCD()
{
    WriteToLCD(0b00111000, 1); // Function Setting  
    WriteToLCD(0b00000110, 1); // Entering Mode   
    WriteToLCD(0b00001101, 1); // Display on 
    WriteToLCD(0b00000001, 1); // Clear Screen 
    WriteToLCD(0b10000000, 1); // Move to top left
}*/





