////////////////////////////////////////
// i2c sniffer by rricharz (tft version)
////////////////////////////////////////

// For Arduino Mega 2560 (8 kB RAM required)
// Uses direct port access for maximum speed
// Do not change pin assignments unless you know what you are doing!
// They are hardcoded in SAMPLE, gbuffer(), START1 and START2 for maximum speed

// The program has been tested sniffing the i2c bus of a Raspberry Pi i2c master running at 100 kBaud

// The data is displayed in the Serial Monitor, set to 115200 baud

#define SDA               0x80                        // Pin 30 (PC7) on PINC is used for the SDA line
#define SCL               0x40                        // Pin 31 (PC6) on PINC is used for the SCL line
#define SAMPLE            (PINC & 0xC0)               // both pins
#define START1            0xC0                        // start condition: transition from START1 to START2
#define START2            0x40

#define BUFFSIZE          5000

#define TIMEOUT           10000                       // timeout in msec, change as required

byte buffer[BUFFSIZE];

////////////
void setup()
////////////
{
  Serial.begin(115200);
  Serial.println("\ni2c sniffer by rricharz\n");
  int points = acquire_data();
  display_data(points);
}

////////////////////////
void printHexByte(int b)
////////////////////////
// Startdard HEX printing does not print leading 0
{
  // Serial.print("0x");
  Serial.print((b >> 4) & 0xF, HEX);
  Serial.print(b & 0xF, HEX);
}

//////////////////
int acquire_data()
//////////////////
//
// Pins are hard coded for speed reason
{
  unsigned long endtime;
  unsigned int data,lastData;

  Serial.println("Acquiring data");

  // wait until start condition is fullfilled
  bool start = false;
  while (!start) {
    while ((lastData = SAMPLE) != START1);          // wait until state is START1
    while ((data = SAMPLE) == lastData);            // wait until state change
    start = (data == START2);                       // start condtion met if new state is START2
  }

  endtime = millis() + TIMEOUT;
  lastData = START2;
  int k = 0;
  buffer[k++] = START1;
  buffer[k++] = START2;
  do {
    
    while ((data = SAMPLE) == lastData);           // wait until data has changed
    buffer[k++] = lastData = data;
  }
  while ((k < BUFFSIZE) && (millis() < TIMEOUT));
  return k;
}

//////////////////
int gbuffer(int i)
//////////////////
{
    return (buffer[i] >> 6) & 0x3;
}

/////////////////////////////////
int findNextStartCondition(int k)
/////////////////////////////////
{
  while((k < BUFFSIZE- 1) && ((gbuffer(k - 1) != 3) || (gbuffer(k) != 1)))
    k++;
    // Serial.print("Next start condition: "); Serial.println(k);
  return k;
}

/////////////////////////////
void display_data(int points)
/////////////////////////////
{
  int lastData, data, k, Bit, Byte, i, state,nextStart;

#define ADDRESS  0         // First state, address follows

  Serial.print("Raw transitions, number of transitions = ");
  Serial.println(points);
  Serial.print("Each number represents a status, bit 1 = SDA, bit 0 = CLK, start condition = 31");
  for (k = 0; k < points; k++) {
    if ((gbuffer(k) == 3) && (gbuffer(k + 1) == 1))        // start condition
      Serial.println();
    Serial.print(gbuffer(k));
  }

  Serial.println();
  Serial.print("Analyzing data, number of transitions = "); Serial.println(points);
  Serial.println("i2c bus activity: * means ACQ = 1 (not ok)");
  k = 3;              // ignore start transition
  i = 0;
  Byte = 0;
  state = ADDRESS;
  nextStart = findNextStartCondition(k);
  do {
    data = gbuffer(k++);
    if (data & 1) {
      Bit = (data & 2) > 0;
      Byte = (Byte << 1) + Bit;
      if (i++ >= 8) {
        if (state == ADDRESS) {
          Serial.print("Dev=");
          printHexByte(Byte / 4);
          if (Byte & 2)
            Serial.print(" R");
          else
            Serial.print(" W");
          if (Byte & 1)
            Serial.print("*");
          else
            Serial.print(" ");
          state++;
        }       
        else if (state == 1) {
          Serial.print("Data=");
          printHexByte(Byte / 2);
          if (Byte & 1)
            Serial.print("*");
          else
            Serial.print(" ");
          state++;
          k--;                      // why ???
        }
        else {
          printHexByte(Byte / 2);
          if (Byte & 1)
            Serial.print("*");
          else
            Serial.print(" ");
        }
        if (nextStart - k < 9) {
          Serial.println();
          k = nextStart + 1;
          nextStart = findNextStartCondition(k);
          state = ADDRESS;
        }
        i = 0;
        Byte = 0;
      }
    }
  }
  while (k < points);
  Serial.println();
}

///////////
void loop()
///////////
{
}
