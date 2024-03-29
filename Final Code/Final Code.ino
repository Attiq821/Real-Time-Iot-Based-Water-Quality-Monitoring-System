#include <LiquidCrystal.h>
LiquidCrystal lcd(19, 23, 18, 17, 16, 15);

/*------------------------------------TDS_Start-----------------------------------------*/
const int TdsSensorPin = 34;
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;
/*------------------------------------TDS_End-----------------------------------------*/

float qi_1=0;
float qi_2=0;
float qi_3=0;
float qi_4=0;
float sum;
float wi;
float wqi;
/*------------------------------------Temp_Start-----------------------------------------*/
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 4;    
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
/*------------------------------------Temp_End------------------------------------------*/ 

#include <WiFi.h>

String apiWritekey = "K7CJ91UJP43QWUV5"; 
const char* ssid = "Attiq";
const char* password = "123456789";

const char* server = "api.thingspeak.com";
float resolution=3.3/1023;
WiFiClient client;

/*------------------------------------Turbidity_Start-----------------------------------*/
const int sensorPin = 35;   // analogue pin of arduino
float volt;
float ntu;
/*------------------------------------Turbidity_End-------------------------------------*/


/*----------------------------------------pH_Start--------------------------------------------*/
const int SensorPin = 32;         //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
/*----------------------------------------pH_End--------------------------------------------*/


void setup()
{
      lcd.begin(16, 2);
      
      Serial.begin(115200);
      sensors.begin();
      delay(1000);
      
      /*-----------------------------ThigSpeak_Start---------------------*/
      WiFi.disconnect();
      delay(10);
      WiFi.begin(ssid, password);  
      Serial.println();
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
    
      pinMode(TdsSensorPin,INPUT);
      
      while (WiFi.status() != WL_CONNECTED)  
      {
        delay(500);
        Serial.print(".");
      }
      /*-----------------------------ThigSpeak_End---------------------*/
}

void loop()
{
 
  /*------------------------------------TDS_Start-----------------------------------------*/  
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
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4095.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      
      //Serial.print("Compensation Volatge:");
      //Serial.print(compensationVolatge,0);
      //Serial.print("V ---->");
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //tdsValue=tdsValue/8;
      
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS: ");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
      delay(1000);
   }
   /*------------------------------------TDS_End-----------------------------------------*/

    
    /*------------------------------------Temp_Start-----------------------------------------*/    
    sensors.requestTemperatures(); 
    float temperatureC = sensors.getTempCByIndex(0);
    float temperatureF = sensors.getTempFByIndex(0);
    Serial.print("Temp: ");
    Serial.print(temperatureC);  
    Serial.print("ºC ---> ");
    Serial.print(temperatureF);
    Serial.println("ºF");
    delay(1000);   
    /*------------------------------------Temp_End-----------------------------------------*/

    /*------------------------------------Turbidity_Start-----------------------------------------*/
    volt = 0;
    for(int i=0; i<800; i++)                              // Loop to take 800 readings
    {
        volt += ((float)analogRead(sensorPin)/4095)*3.3;    // Read the input on analog pin 0 & Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
     }
    volt = volt/800;                                      // Taking the Average of 800 Readings
    volt = round_to_dp(volt,1);                           // Calling the "round_to_dp" method to round off the volt reading
    volt=volt+2.65;   
    
    if(volt < 2.5)
    {                                       // If volt is less than 2.5 then run the block  
      ntu = 3000;                                         // Set the Turbidity value to 3000       
    }
    else
    {                                                
     ntu = -1120.4*(volt*volt)+5742.3*volt-4353.8;      // If the volt is above 2.5 then calculate the NTU value by using the equation give in the graph 
     //ntu=ntu+191.25;
     //ntu=ntu+580.56+406.72+5;
     ntu=ntu+580.56+406.72+5-806+5;
    }
    
    //Serial.println("Voltage="+String(volt)+" V --------> Turbidity="+String(ntu)+" NTU");  // Printing results on serial moniter
    Serial.println("Turbidity = "+String(ntu)+" NTU");  // Printing results on serial moniter
    
    delay(1000);                 
    /*------------------------------------Turbidity_End-----------------------------------------*/

    /*----------------------------------------pH_Start--------------------------------------------*/
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
    float phValue=(float)avgValue*3.3/4095/6; //convert the analog into millivolt
    phValue=5+phValue;  
    
    //convert the millivolt into pH value
    Serial.print("pH: ");  
    Serial.print(phValue,2);
    Serial.println(" ");
    delay(1000);
    Serial.println(" ");
    Serial.println(" ");
    /*----------------------------------------pH_End--------------------------------------------*/
   
   /*-----------------------------------WQI---------------------------------------------------*/
  if(ntu<10)
  {
    //Serial.println("If NTU");
    qi_1=(ntu*100)/5;
  }
  if(ntu>=10)
  {
    //Serial.println("else NTU");
    qi_1=(ntu*100)/300;
  }
  
  qi_2=(phValue*100)/8.5;
  
  //Serial.print("TDS_Test : ");
  //Serial.println(tdsValue);
  qi_3=(tdsValue*100)/50;

  /*
  if(tdsValue<300)
  {
    //Serial.println("If TDS");
    qi_3=(tdsValue*100)/50;
  }
  if(tdsValue>=300)
  {
    //Serial.println("else TDS");
    qi_3=(tdsValue*100)/100;
  }
  */
  
  qi_4=(temperatureC*100)/30;
  
  /*
  qi_3=(280/300)*100;   //93.33
  qi_4=(25/30)*100;     //83.33
  qi_1=(5/5)*100;       //100
  qi_2=(7.5/8.5)*100;   //88.24
  */

  wi=((1/5.0)+(1/8.5)+(1/300.0)+(1/30.0));  //0.354

  sum=((qi_1/5)+(qi_2/8.5)+(qi_3/300)+(qi_4/30)); 

  wqi= sum/wi;
  
  Serial.print("WQI : ");
  Serial.println(wqi);
  

  //Serial.print("Qi_1 : ");
  //Serial.println(qi_1);
  //Serial.print("Qi_2 : ");
  //Serial.println(qi_2);
  //Serial.print("Qi_3 : ");
  //Serial.println(qi_3);
  //Serial.print("Qi_4 : ");
  //Serial.println(qi_4);

  //Serial.print("wi : ");
  //Serial.println(wi);
  //Serial.print("sum : ");
  //Serial.println(sum);

  
  /*-------------------------------------WQI_END----------------------------------------*/
  
    /*-----------------------------ThigSpeak_Start---------------------*/

    if (client.connect(server,80)) {
      String tsData = apiWritekey;
      tsData +="&field1=";
      tsData += String(tdsValue);
      tsData +="&field2=";
      tsData += String(phValue);
      tsData +="&field4=";
      tsData += String(temperatureC);
      tsData +="&field5=";
      tsData += String(ntu);
      tsData +="&field6=";
      tsData += String(wqi);
      tsData += "\r\n\r\n";
  
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: "+apiWritekey+"\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      
      client.print(tsData.length());
      client.print("\n\n");
      client.print(tsData);
     
       //client.print(tsData.length());
       //client.print("\n\n");   
       //client.print(tsData);
       
      Serial.println("uploaded to Thingspeak server....");
    }
    Serial.println("Waiting to upload next reading...");
    Serial.println();
  /*-----------------------------ThigSpeak_End---------------------*/

  /*-----------------------------LCD_Start---------------------*/

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 0);
    // print the number of seconds since reset:
    //lcd.print(millis() / 1000);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("TDS: ");
    lcd.print(tdsValue);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(phValue);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperatureC);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Turbidity: ");
    lcd.print(ntu);
    delay(1000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("WQI: ");
    lcd.print(wqi);
    delay(1000);
    lcd.clear();

  if (wqi <50)
  {
    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Quality: Excellent");
    delay(1000);
    lcd.clear();
  }
  else if (wqi>=50 &&  wqi<100)
  {
    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Quality: Good");
    delay(1000);
    lcd.clear();
  }
  else if (wqi>=100 &&  wqi<200)
  {
    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Quality: Poor");
    delay(1000);
    lcd.clear();
  }
  else if (wqi>=200 &&  wqi<300)
  {
    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Quality: Very Poor");
    delay(1000);
    lcd.clear();
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("IoT Water Quality");
    lcd.setCursor(0, 1);
    lcd.print("Quality: Hazardous");
    delay(1000);
    lcd.clear();
  }
  Serial.println(" ");
  /*-----------------------------LCD_End---------------------*/

}


/*------------------------------------TDS_Start-----------------------------------------*/
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
/*------------------------------------TDS_End-----------------------------------------*/

/*------------------------------------Turbidity_Start-----------------------------------------*/
float round_to_dp( float in_value, int decimal_place )    // Method to round of the volt value
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
/*------------------------------------Turbidity_End-----------------------------------------*/
