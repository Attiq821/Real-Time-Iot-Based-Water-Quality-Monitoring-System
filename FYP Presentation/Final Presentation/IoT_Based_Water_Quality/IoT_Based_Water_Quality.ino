
/*-------------------------------------TDS Sensor_Start-------------------------------------------*/

//TDS Sensor

#define TdsSensorPin A0
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT 30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;

//Turbidity Sensor

#define TurbiditySensorPin A1   // analogue pin of arduino
float volt;
float ntu;

//ph Sensor

#define SensorPin A0          //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
void setup()
{
    Serial.begin(115200);
    pinMode(TdsSensorPin,INPUT);
    pinMode(TurbiditySensorPin,INPUT);
}

void loop()
{
/*------------------------------------TDS_Sensor_Start-------------------------------------------*/
 
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
   }
/*-------------------------------------TDS_Sensor_End-------------------------------------------*/
 
   
/*-------------------------------------Turbidity_Sensor_Start-------------------------------------------*/  
   volt = 0;
    for(int i=0; i<800; i++)                              // Loop to take 800 readings
    {
        volt += ((float)analogRead(TurbiditySensorPin)/1023)*5;    // Read the input on analog pin 0 & Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    }
    volt = volt/800;                                      // Taking the Average of 800 Readings
    volt = round_to_dp(volt,1);                           // Calling the "round_to_dp" method to round off the volt reading
    if(volt < 2.5){                                       // If volt is less than 2.5 then run the block  
      ntu = 3000;                                         // Set the Turbidity value to 3000       
    }else{                                                
      ntu = -1120.4*square(volt)+5742.3*volt-4353.8 - 1795;      // If the volt is above 2.5 then calculate the NTU value by using the equation give in the graph 
    }
    Serial.println("Voltage="+String(volt)+" V --------> Turbidity="+String(ntu)+" NTU");  // Printing results on serial moniter
    Serial.println("");  
    Serial.println("");      
    delay(3000);
/*-------------------------------------Turbidity_Sensor_End-------------------------------------------*/

/*-------------------------------------pH_Sensor_Start-------------------------------------------*/
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;  
 
  //convert the millivolt into pH value
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" ");
  delay(1000);
/*-------------------------------------pH_Sensor_End-------------------------------------------*/
}


/*-------------------------------------TDS_Sensor_Function_Start-------------------------------------------*/
int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
/*-------------------------------------TDS_Sensor_Function_End-------------------------------------------*/

/*-------------------------------------Turbidity_Sensor_Function_Start-------------------------------------------*/
float round_to_dp( float in_value, int decimal_place )    // Method to round of the volt value
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
/*-------------------------------------Turbidity_Sensor_Function__End-------------------------------------------*/



