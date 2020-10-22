//Incluimos librerías.
//CÓDIGO PARA LA SD CARD.
#include <SPI.h> //Librería para la SD Card.
#include <SD.h> //Librería para la SD Card.
//CÓDIGO PARA EL MPU6050.
#include <Wire.h> //Librería para interactuar con el MPU6050.
#include <MPU6050.h> //Librería para el MPU6050.
//CÓDIGO PARA EL GPS.
#include <SoftwareSerial.h>//Incluimos SoftwareSerial para comunicarnos con el GPS
#include <TinyGPS.h>//Librería para comunicarse con el GPS.

//CÓDIGO PARA LA SD CARD.
const int chipSelect = 10; //Variable para leer la SD Card.

//LED DE FALLOS
const int led_fallo = 7; //Variable para encender el led cuando ocurre una falla.

//CÓDIGO PARA EL MPU6050.
MPU6050 mpu; //Instanciamos el objeto MPU6050.

//CÓDIGO PARA EL PULSADOR.
  const int pin_pulsador=2; //Pulsador para identificar baches.
  bool pulsador_status;

//CÓDIGO PARA EL GPS.
TinyGPS gps;//Declaramos el objeto gps
SoftwareSerial serialgps(4,3);//Declaramos el pin 4 Tx y 3 Rx
float latitude, longitude; //Variables de localización del GPS.
float latitude_gps, longitude_gps; //Variables de localización del GPS.

void setup() {
//CÓDIGO PARA INICIALIZAR LA SD CARD.
// Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(led_fallo,OUTPUT);//Ponemos como salida el pin del led de fallo.
  digitalWrite(led_fallo,LOW); //Iniciamos el pin del led en un estado bajo.
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    digitalWrite(led_fallo,HIGH);// don't do anything more:
    return;
  }
  else {Serial.println("SD Card Inicializada");}

//CÓDIGO PARA INICIAR EL MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_4G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    digitalWrite(led_fallo,HIGH);
    delay(1000);
    digitalWrite(led_fallo,LOW);
    delay(1000);
  }


//INICIAMOS EL PULSADOR.
  pinMode(pin_pulsador,INPUT);
  digitalWrite(pin_pulsador,LOW);
  Serial.println("Pulsador listo");

//CÓDIGO PARA EL GPS.
  serialgps.begin(9600);//Iniciamos el puerto serie del gps
  //Imprimimos algunas alertas
  Serial.println("");
  Serial.println("GPS GY-GPS6MV2 Leantec");
  Serial.println(" ---Buscando senal--- ");
  Serial.println("");
}

void loop() {
  String dataString="";
// CÓDIGO PARA LEER LA DATA DEL MPU6050:
  Vector normAccel = mpu.readNormalizeAccel();
  Vector normGyro = mpu.readNormalizeGyro();
  float  accXAxis=normAccel.XAxis;
  float  accYAxis=normAccel.YAxis;
  float  accZAxis=normAccel.ZAxis;

  float  gyroXAxis=normGyro.XAxis;
  float  gyroYAxis=normGyro.YAxis;
  float  gyroZAxis=normGyro.ZAxis;

//LEEMOS LA DATA DEL PULSADOR.
  pulsador_status=digitalRead(pin_pulsador);
  Serial.println(pulsador_status);

//LEEMOS LA DATA DEL GPS.
  while(serialgps.available()) 
  {
    int c = serialgps.read();
 
    if(gps.encode(c))  
    {
      gps.f_get_position(&latitude, &longitude);
      Serial.print("Latitud/Longitud: "); 
      Serial.print(latitude,10); 
      Serial.print(", "); 
      Serial.println(longitude,10);
      latitude_gps=latitude;
      longitude_gps=longitude;
      }
  }
  Serial.print("Latitud: ");
  Serial.println(latitude_gps);
  Serial.print("Longitud: ");
  Serial.println(longitude_gps);

//ESCRIBIMOS LA DATA EN EL SD CARD.
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("dato.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(accXAxis,10);
    dataFile.print(",");
    dataFile.print(accYAxis,10);
    dataFile.print(",");
    dataFile.print(accZAxis,10);
    dataFile.print(",");
    dataFile.print(gyroXAxis,10);
    dataFile.print(",");
    dataFile.print(gyroYAxis,10);     
    dataFile.print(",");
    dataFile.print(gyroZAxis,10);
    dataFile.print(",");
    dataFile.print(pulsador_status);
    dataFile.print(",");
    dataFile.print(latitude,10);
    dataFile.print(",");
    dataFile.println(longitude,10);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(stringVal);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
    digitalWrite(led_fallo,HIGH);
    delay(200);
    digitalWrite(led_fallo,LOW);
    delay(200);
  }
  delay(50);//Lectura de la data a 20Hz.
}
