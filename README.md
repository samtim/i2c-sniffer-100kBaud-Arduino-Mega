# i2c-sniffer-100kBaud-Arduino-Mega

This Arduino Mega sketch uses direct port read and bit manipulations
to sniff an i2c bus running at up to 100 kHz.

The analysis of the i2c communication is displayed on the Serial monitor.

Connect SDA to pin 30 of the Arduino Mega, and SCL to pin 31.

The Aduino is not fast enough to acquire and display the data at the same time.
This is usually not a problem because the i2c access normally happens in bursts.
Set TIMEOUT longer than the maximum length of one burst, but shorter than
the time beween bursts minus the time required to analye and display the data.

Example: maximum time of one i2c burst  300 msec.
Time between bursts: 3000 msec.
Time to analyze and display data of one burst: 100 msec.
In this example TIMEOUT can be set between 300 msec and 2900 msec

If you don't know anything about the i2c activity, set the timeout initially to a
large value, for example 10000 msec.
This means that the sampling will go on for 10000 msec after a first i2c start
condition has been observed.

A sample output https://github.com/rricharz/i2c-sniffer-100kBaud-Arduino-Mega/blob/master/sniffer.txt is enclosed.

Please open an issue in this repository if you have any problem with the program.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
