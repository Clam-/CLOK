// all the alarm clock functionality: display, alarm, etc...

// hmm how to represent hour and minute... I think probably an hour characteristic and a minute characteristic would be simplest
// could double stack and put 2 numbers in one byte, like x & 0xFFFF, x >> 16 & 0xFFFF
BLECharCharacteristic alarmhour("BAAD0005-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);
BLECharCharacteristic alarmminute("BAAD0005-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);
// I wonder how I should try and make different sounds...
BLEByteCharacteristic alarmsound("BAAD0006-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);
// this in reality is 0-15 based on MAX7221 chip 'intensity' register
BLECharCharacteristic brightness("BAAD0007-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLEWrite);

// wonder how I should send this...
BLEByteCharacteristic localdatetime("BAAD0007-5AAD-BAAD-FFFF-5AD5ADBADCLK", BLERead | BLENotify);