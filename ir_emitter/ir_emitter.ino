// -----------------------------------
// Controlling LEDs over the Internet
// -----------------------------------

// name the pins
int IRledPin = 13;

// This routine runs only once upon reset
void setup()
{
   //Register our Spark function here
   Spark.function("ir", irControl);

   // Configure the pins to be outputs
   pinMode(IRledPin, OUTPUT);

   // Initialize the LED to be OFF
   digitalWrite(IRledPin, LOW);
}


// This routine loops forever 
void loop()
{
   // Nothing to do here
   irControl("10010");
   delay(1000);
   irControl("00010");
   delay(1000);
   irControl("01010");
   delay(1000);
   irControl("11010");
   delay(1000);
}


// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }
 
  sei();  // this turns them back on
}


void bit_out(int b){
   pulseIR(600);

   if (b == 0){
      delayMicroseconds(560);
   } else if (b == 1){
      delayMicroseconds(1640);
   } else {
      Serial.println("Found a strange bit: " + b);
   }
}


void SendChannelUpCode(int bits[5]){

   // Beginning
   pulseIR(8880);
   delayMicroseconds(4400);

   for (int i=0; i<8; i++){ // 00000000
      bit_out(0);
   }
   for (int i=0; i<8; i++){ // 11111111
      bit_out(1);
   }

   // Middle
   for (int i=0; i<5; i++){ // bbbbb
      int b = bits[i];
      bit_out(b);
   }

   for (int i=0; i<3; i++){ // 000
      bit_out(0);
   }

   for (int i=0; i<5; i++){ // ppppp
      int b = (bits[i] + 1) % 2;
      bit_out(b);
   }

   // End
   for (int i=0; i<3; i++){ // 111
      bit_out(1);
   }

   pulseIR(600);
   delayMicroseconds(39040);
   pulseIR(8900);
   delayMicroseconds(2160);
   pulseIR(600);
}

// This function gets called whenever there is a matching API request
// the command string format is <bit 1><bit 2><bit 3><bit 4><bit 5>
// for example: 10101 or 10110

void irControl(String command)
{
   int bits[5] = {0,0,0,0,0};

   for (int i=0; i<5; i++){
      bits[i] = command.charAt(i) - '0';
   }

   SendChannelUpCode(bits);

   return;
}
