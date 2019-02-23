// Programa : 
// Autor : Leonardo (Acqua Nativa)

/*
 


*/

#include<SoftwareSerial.h>

void LCM_write(unsigned int VP, float VL) ; // protótipo de LCM_write

const int analogInPin = A0; 
const int Rx_nano=11 ;
const int Tx_nano=12 ;

float sensorValue=0;

SoftwareSerial soft_Serial(Rx_nano, Tx_nano); // UART para comunicacao com o AD LCM

void setup()
{
 Serial.begin(9600); // UART/USB 
 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

 soft_Serial.begin(115200) ; // msm baud rate do AD LCM
}


void loop() {
   sensorValue = (( (float) analogRead(analogInPin) )/1023)*5 ; // ADC de 10 bits [0-1023]
   
Serial.print("value= ");
Serial.println(sensorValue);

LCM_write(0x0000,sensorValue) ; // write a partir do VP=x0000 o valor do float do segundo argumento convertido para ASCII (cada VP armazena 2 bytes[dois char])

delay(250); // mede o valor de orp a cada 250 ms.

}

void LCM_write(unsigned int VP, float VL) {
  
 // ESTA FUNÇÃO ESCREVE um float(4 bytes no Arduino Uno) a partir de VP (Escreve VL  em VP e VP+1)
  // VP : endereco da RAM onde escreverá VL (VP armazena 2 bytes); int (ou unsigned int tem 2 bytes no Arduino UNO)
  
 // This Command (Function) writes one or more VPs. You can write multiple VPS at once, if they are sequential.
  // Format: <FHH> <FHL> <BC> 82 <VP><VP> <VL1><VL1> [<VL2><VL2> <VL3><VL3> ...] 
 // EX: Write values on 4 sequential VPs, starting from VP 0x0000:
// => 5AA5 0B82 0000 0022 0071 0006 0031          //(olhar Development Guide do proculus AD LCM)
// o tipo byte é equivalente a unsigned char
  
  byte FHH=0x5A;
  byte FHL=0xA5;
  byte BC=0x03; // numero de bytes a partir do comando(82) ; nmero minimo de BC=3 pois 1(CMD) + 2(VP) 
  byte CMD=0x82; // comando de escrita em VP(endereço da RAM)

  //<VP><VP> => 0x(VP_HB VP_LB)
  byte VP_HB=  VP >> 8 ;  // High Byte do VP
  byte VP_LB=  VP & (0x00FF) ;  // Low Byte  do VP

// envia o float como caracteres usando código ASCII ; 2 casas decimais + pontodecimal+ 4 casas
String VL_str = String(VL,3);           // using a float with 3 decimal places

byte data[ VL_str.length() +1 ];   // data armazenará os caracteres da string VL_str (incluindo o caracter Nulo)

VL_str.getBytes(data,sizeof(data) ) ;                   // Copies the String’s characters to the supplied buffer(data)
BC+= sizeof(data) ; // incrementa BC com o numero de bytes de data

// montando e enviando o packet pela Serial ; 
 byte pacote_LCM[6 + sizeof(data) ]={FHH,FHL,BC,CMD, VP_HB,VP_LB} ; // pacote para escrever a string data a partir de VP

for(int i=0; i<sizeof(data) ; i++) pacote_LCM[i+6]= data[i] ; // coloca data no pacote_LCM
 
 size_t nB_pacote_LCM= sizeof(pacote_LCM) ;   // numero de bytes no pacote_LCM
 soft_Serial.write(pacote_LCM, nB_pacote_LCM) ;   // envia o pacote_LCM 
}

  
