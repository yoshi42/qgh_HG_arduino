#include <DFPlayer_Mini_Mp3.h> // Бібліотека МП3хи
byte redLed = 6;
byte greenLed = A5;
byte blueLed = 5;
//byte load = 13;
byte codeLed = 5;

byte micPin = A1;

byte enIn = A0;
byte enOut = A4;
byte rellay = A3;

int dotDuration = 200; //dot duration time
int listenTime = 20;//listen time in seconds
int threshold = 150;

//String codeWord[3] = "SOS ";//code word

boolean listenFlag = false;

static const struct {const char letter, *code;} MorseMap[] =
{
	{ 'A', ".-" },
	{ 'B', "-..." },
	{ 'C', "-.-." },
	{ 'D', "-.." },
	{ 'E', "." },
	{ 'F', "..-." },
	{ 'G', "--." },
	{ 'H', "...." },
	{ 'I', ".." },
	{ 'J', ".---" },
	{ 'K', ".-.-" },
	{ 'L', ".-.." },
	{ 'M', "--" },
	{ 'N', "-." },
	{ 'O', "---" },
	{ 'P', ".--." },
	{ 'Q', "--.-" },
	{ 'R', ".-." },
	{ 'S', "..." },
	{ 'T', "-" },
	{ 'U', "..-" },
	{ 'V', "...-" },
	{ 'W', ".--" },
	{ 'X', "-..-" },
	{ 'Y', "-.--" },
	{ 'Z', "--.." },
	{ ' ', "     " }, //Gap between word, seven units 
		
	{ '1', ".----" },
	{ '2', "..---" },
	{ '3', "...--" },
	{ '4', "....-" },
	{ '5', "....." },
	{ '6', "-...." },
	{ '7', "--..." },
	{ '8', "---.." },
	{ '9', "----." },
	{ '0', "-----" },
		
	{ '.', ".-.-.-" },
	{ ',', "--..--" },
	{ '?', "..--.." },
	{ '!', "-.-.--" },
	{ ':', "---..." },
	{ ';', "-.-.-." },
	{ '(', "-.--." },
	{ ')', "-.--.-" },
	{ '"', ".-..-." },
	{ '@', ".--.-." },
	{ '&', ".-..." },
};


void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);	
	pinMode(redLed,OUTPUT);
	pinMode(greenLed,OUTPUT);
	//pinMode(load,OUTPUT);
	pinMode(codeLed,OUTPUT);
	
	pinMode(enOut,OUTPUT);
	pinMode(rellay,OUTPUT);
	
	pinMode(enIn,INPUT);
	pinMode(micPin,INPUT);
	
	digitalWrite(enOut,LOW);
	digitalWrite(rellay,LOW);
	
	digitalWrite(codeLed,HIGH);
//	digitalWrite(load,LOW);
	digitalWrite(redLed,HIGH);
	digitalWrite(greenLed,HIGH);
//	while !(digitalRead(enIn))
	{
		Serial.print(".");
	}
}

void loop() {
	say();
	delay(1500);
	listen();
	/*if (listenFlag)
	{
		digitalWrite(redLed,LOW);
		digitalWrite(greenLed,LOW);
		digitalWrite(load,HIGH);
		infLoop();
	}*/
  // put your main code here, to run repeatedly:

}

void say()
{
	//set led state
	String sayString = encode("O");
	Serial.println(sayString);
	for(int i=0;i<=sayString.length();i++)
	{
		switch(sayString[i])
		{
		case '.': //dot
			digitalWrite(redLed,HIGH);
			digitalWrite(greenLed,HIGH);
			digitalWrite(blueLed,HIGH);
			delay(dotDuration);
			digitalWrite(redLed,LOW);
			digitalWrite(greenLed,LOW);
			digitalWrite(blueLed,LOW);
			delay(dotDuration);
		break;
		case '-': //dash
			digitalWrite(redLed,HIGH);
			digitalWrite(greenLed,HIGH);
			digitalWrite(blueLed,HIGH);
			delay(dotDuration*3);
			digitalWrite(redLed,LOW);
			digitalWrite(greenLed,LOW);
			digitalWrite(blueLed,LOW);
			delay(dotDuration);  
		break;
		//case ' ': //gap
		//	delay(dotDuration);
    }
  }	
}

String encode(const char *string)
{  
	size_t i, j;
	String morseWord = "";
	for( i = 0; string[i]; ++i )
	{
		for( j = 0; j < sizeof MorseMap / sizeof *MorseMap; ++j )
		{
			if( toupper(string[i]) == MorseMap[j].letter )
			{
				morseWord += MorseMap[j].code;
				break;
			}
		}
		//morseWord += " "; //Add tailing space to seperate the chars
	}
	return morseWord;
}

String decode(String morse)
{
	String msg = ""; 
	int lastPos = 0;
	int pos = morse.indexOf(' ');
	while( lastPos <= morse.lastIndexOf(' ') )
	{    
		for( int i = 0; i < sizeof MorseMap / sizeof *MorseMap; ++i )
		{
			if( morse.substring(lastPos, pos) == MorseMap[i].code )
			{
				msg += MorseMap[i].letter;
			}
		}
		lastPos = pos+1;
		pos = morse.indexOf(' ', lastPos);
    
		// Handle white-spaces between words (7 spaces)
		while( morse[lastPos] == ' ' && morse[pos+1] == ' ' )
		{
			pos ++;
		}
	}
	return msg;
} 
 void listen()
{
	unsigned long millisNow = 0;
	unsigned long millisPrew = 0;
	boolean knockFlag = false;
	boolean first = true;
	digitalWrite(greenLed,HIGH);
	String listenString = "";
	long diff = 0;
	for (int i = 0;i<=2500;i++)
	{
		if (analogRead(micPin)>200)
		{
			if (first)
			{
				first = false;
				millisNow = millis();	
				millisPrew = millisNow;
			}
			millisPrew = millisNow;
			millisNow = millis();
			digitalWrite(blueLed,HIGH);
			digitalWrite(redLed,HIGH);			
			delay(100);
			digitalWrite(blueLed,LOW);
			digitalWrite(redLed,LOW);
			knockFlag = true;
			diff = millisNow-millisPrew;			
			Serial.print("diff = ");
			Serial.println(diff);
		}

		if ((diff>=dotDuration-threshold)&&(diff<=dotDuration+threshold)&&knockFlag)
		{
			listenString=listenString+'.';
			knockFlag = false;
			millisPrew = millisNow;
		}
		if ((diff>=dotDuration*3-threshold)&&(diff<=dotDuration*3+threshold)&&knockFlag)
		{

			listenString=listenString+'-';
			knockFlag = false;
			millisPrew = millisNow;
		}
	delay(2);
	}
	Serial.println("Listen string is:");
	Serial.println(listenString);
	String cmpString = "--"; // 
	boolean  result=(listenString == cmpString);
	Serial.print("cmpResult ");
	Serial.println(result);
	if (result)
	{
		openChest();
	}
}
void openChest ()
{
	digitalWrite(enOut,HIGH);
	digitalWrite(rellay,HIGH);
	digitalWrite(redLed,LOW);
	digitalWrite(greenLed,LOW);
	digitalWrite(blueLed,LOW);
	infLoop();
	
	
}
void infLoop()
{
	while (1) {
	for(int i = 0;i<=255;i++)
	{
		analogWrite(redLed,i);
		analogWrite(blueLed,255-i);
		delay(10);
	}
	for(int i = 255;i>=0;i--)
	{
		analogWrite(redLed,i);
		analogWrite(blueLed,255-i);
		delay(10);
	}
	}
}

