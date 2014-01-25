#include "KnxTelegram.h"

KnxTelegram::KnxTelegram() {
	clear();
}

void KnxTelegram::clear() {
	for (int i = 0; i < MAX_KNX_TELEGRAM_SIZE; i++) {
		buffer[i] = 0;
	}

	// Control Field, Normal Priority, No Repeat
	buffer[0] = B10111100;

	// Target Group Address, Routing Counter = 6, Length = 1 (= 2 Bytes)
	buffer[5] = B11100001;
}

int KnxTelegram::getBufferByte(int index) {
	return buffer[index];
}

void KnxTelegram::setBufferByte(int index, int content) {
	buffer[index] = content;
}

bool KnxTelegram::isRepeated() {
	// Parse Repeat Flag
	if (buffer[0] & B00100000) {
		return false;
	} else {
		return true;
	}
}

void KnxTelegram::setRepeated(bool repeat) {
	if (repeat) {
		buffer[0] = buffer[0] & B11011111;
	} else {
		buffer[0] = buffer[0] | B00100000;
	}
}

void KnxTelegram::setPriority(KnxPriorityType prio) {
	buffer[0] = buffer[0] & B11110011;
	buffer[0] = buffer[0] | (prio << 2);
}

KnxPriorityType KnxTelegram::getPriority() {
	// Priority
	return (KnxPriorityType) ((buffer[0] & B00001100) >> 2);
}

void KnxTelegram::setSourceAddress(int area, int line, int member) {
	buffer[1] = (area << 4) | line;	// Source Address
	buffer[2] = member; // Source Address
}

int KnxTelegram::getSourceArea() {
	return (buffer[1] >> 4);
}

int KnxTelegram::getSourceLine() {
	return (buffer[1] & B00001111);
}

int KnxTelegram::getSourceMember() {
	return buffer[2];
}

void KnxTelegram::setTargetGroupAddress(int main, int middle, int sub) {
	buffer[3] = (main << 3) | middle;
	buffer[4] = sub;
	buffer[5] = buffer[5] | B10000000;
}

void KnxTelegram::setTargetIndividualAddress(int area, int line, int member) {
	buffer[3] = (area << 4) | line;
	buffer[4] = member;
	buffer[5] = buffer[5] & B01111111;
}

bool KnxTelegram::isTargetGroup() {
	return buffer[5] & B10000000;
}

int KnxTelegram::getTargetMainGroup() {
	return ((buffer[3] & B01111000) >> 3);
}

int KnxTelegram::getTargetMiddleGroup() {
	return (buffer[3] & B00000111);
}

int KnxTelegram::getTargetSubGroup() {
	return buffer[4];
}

int KnxTelegram::getTargetArea() {
	return ((buffer[3] & B11110000) >> 4);
}

int KnxTelegram::getTargetLine() {
	return (buffer[3] & B00001111);
}

int KnxTelegram::getTargetMember() {
	return buffer[4];
}

void KnxTelegram::setRoutingCounter(int counter) {
	buffer[5] = buffer[5] & B10000000;
	buffer[5] = buffer[5] | (counter << 4);
}

int KnxTelegram::getRoutingCounter() {
	return ((buffer[5] & B01110000) >> 4);
}

void KnxTelegram::setPayloadLength(int length) {
	buffer[5] = buffer[5] & B11110000;
	buffer[5] = buffer[5] | (length - 1);
}

int KnxTelegram::getPayloadLength() {
	int length = (buffer[5] & B00001111) + 1;
	return length;
}

void KnxTelegram::setCommand(KnxCommandType command) {
	buffer[6] = buffer[6] & B11111100;
	buffer[7] = buffer[7] & B00111111;

	buffer[6] = buffer[6] | (command >> 2); // Command first two bits
	buffer[7] = buffer[7] | (command << 6); // Command last two bits
}

KnxCommandType KnxTelegram::getCommand() {
	return (KnxCommandType) (((buffer[6] & B00000011) << 2) | ((buffer[7] & B11000000) >> 6));
}

void KnxTelegram::setControlData(KnxControlDataType cd) {
    buffer[6] = buffer[6] & B11111100;
    buffer[6] = buffer[6] | cd;
}

KnxControlDataType KnxTelegram::getControlData() {
    return (KnxControlDataType) (buffer[6] & B00000011);
}

KnxCommunicationType KnxTelegram::getCommunicationType() {
    return (KnxCommunicationType) ((buffer[6] & B11000000) >> 6);
}

void KnxTelegram::setCommunicationType(KnxCommunicationType type) {
    buffer[6] = buffer[6] & B00111111;
    buffer[6] = buffer[6] | (type << 6);
}

int KnxTelegram::getSequenceNumber() {
    return (buffer[6] & B00111100) >> 2;
}

void KnxTelegram::setSequenceNumber(int number) {
    buffer[6] = buffer[6] & B11000011;
    buffer[6] = buffer[6] | (number << 2);
}

void KnxTelegram::setFirstDataByte(int data) {
	buffer[7] = buffer[7] & B11000000;
	buffer[7] = buffer[7] | data;
}

int KnxTelegram::getFirstDataByte() {
	return (buffer[7] & B00111111);
}

void KnxTelegram::createChecksum() {
	int checksumPos = getPayloadLength() + KNX_TELEGRAM_HEADER_SIZE;
	buffer[checksumPos] = calculateChecksum();
}

int KnxTelegram::getChecksum() {
	int checksumPos = getPayloadLength() + KNX_TELEGRAM_HEADER_SIZE;
	return buffer[checksumPos];
}

bool KnxTelegram::verifyChecksum() {
	int calculatedChecksum = calculateChecksum();
	return (getChecksum() == calculatedChecksum);
}

void KnxTelegram::print(TPUART_SERIAL_CLASS* serial) {
	serial->print("Repeated: ");
	serial->println(isRepeated());

	serial->print("Priority: ");
	serial->println(getPriority());

	serial->print("Source: ");
	serial->print(getSourceArea());
	serial->print(".");
	serial->print(getSourceLine());
	serial->print(".");
	serial->println(getSourceMember());

    if (isTargetGroup()) {
        serial->print("Target Group: ");
        serial->print(getTargetMainGroup());
        serial->print("/");
        serial->print(getTargetMiddleGroup());
        serial->print("/");
        serial->println(getTargetSubGroup());
    } else {
        serial->print("Target Physical: ");
        serial->print(getTargetArea());
        serial->print(".");
        serial->print(getTargetLine());
        serial->print(".");
        serial->println(getTargetMember());
    }
        
	serial->print("Routing Counter: ");
	serial->println(getRoutingCounter());

	serial->print("Payload Length: ");
	serial->println(getPayloadLength());

	serial->print("Command: ");
	serial->println(getCommand());

	serial->print("First Data Byte: ");
	serial->println(getFirstDataByte());

	for (int i = 2; i < getPayloadLength(); i++) {
		serial->print("Data Byte ");
		serial->print(i);
		serial->print(": ");
		serial->println(buffer[6+i], BIN);
	}


	if (verifyChecksum()) {
		serial->println("Checksum matches");
	} else {
		serial->println("Checksum mismatch");
		serial->println(getChecksum(), BIN);
		serial->println(calculateChecksum(), BIN);
	}

}

int KnxTelegram::calculateChecksum() {
	int bcc = 0xFF;
	int size = getPayloadLength() + KNX_TELEGRAM_HEADER_SIZE;

	for (int i = 0; i < size; i++) {
		bcc ^= buffer[i];
	}

	return bcc;
}

int KnxTelegram::getTotalLength() {
	return KNX_TELEGRAM_HEADER_SIZE + getPayloadLength() + 1;
}

void KnxTelegram::set2ByteFloatValue(float value) {
	setPayloadLength(4);

	int v = value * 100.0;

	if (v < 0) {
		buffer[8] = B10000000;
		v = abs(v);
	} else {
		buffer[8] = B00000000;
	}

	int exponent = 0;
	while (0xF800 & v) {
		v >>= 1;
		exponent += 1;
	}

	buffer[8] = buffer[8] | ((B1111 & exponent) << 3);
	buffer[8] = buffer[8] | (B00000111 & (v >> 8));
	buffer[9] = v;
}

float KnxTelegram::get2ByteFloatValue() {
	if (getPayloadLength() != 4) {
		// Wrong payload length
		return 0;
	}

	int exponent = (buffer[8] & B01111000) >> 3;
	int mantissa = ((buffer[8] & B00000111) << 8) | (buffer[9]);

	int sign = 1;

	if (buffer[8] & B10000000) {
		sign = -1;
	}

	return (mantissa * 0.01) * pow(2.0, exponent);
}

void KnxTelegram::set4ByteFloatValue(float value) {
  setPayloadLength(6);

  byte b[4];  
  float *f = (float*)(void*)&(b[0]);
  *f=value;

  buffer[8+3]=b[0];
  buffer[8+2]=b[1];
  buffer[8+1]=b[2];
  buffer[8+0]=b[3];
}

float KnxTelegram::get4ByteFloatValue() {
    if (getPayloadLength() != 6) {
        // Wrong payload length
        return 0;
    }
  byte b[4];
  b[0]=buffer[8+3];
  b[1]=buffer[8+2];
  b[2]=buffer[8+1];
  b[3]=buffer[8+0];
  float *f=(float*)(void*)&(b[0]);
  float  r=*f;
  return r;
}


void KnxTelegram::set14ByteValue(String value) {
  // load definieren
  char _load[15];
  
  // load mit space leeren/initialisieren
  for (int i=0; i<14; ++i)
  {_load[i]= 0;}
  setPayloadLength(16);
  //mache aus Value das CharArray
  value.toCharArray(_load,15); // muss 15 sein - weil mit 0 abgeschlossen wird
  buffer[8+0]=_load [0];
  buffer[8+1]=_load [1];
  buffer[8+2]=_load [2];
  buffer[8+3]=_load [3];
  buffer[8+4]=_load [4];
  buffer[8+5]=_load [5];
  buffer[8+6]=_load [6];
  buffer[8+7]=_load [7];
  buffer[8+8]=_load [8];
  buffer[8+9]=_load [9];
  buffer[8+10]=_load [10];
  buffer[8+11]=_load [11];
  buffer[8+12]=_load [12];
  buffer[8+13]=_load [13];
}

//
String KnxTelegram::get14ByteValue(String value) {
if (getPayloadLength() != 16) {
        // Wrong payload length
        return 0;
    }
    char _load[15];
	_load[0]=buffer[8+0];
	_load[1]=buffer[8+1];
	_load[2]=buffer[8+2];
	_load[3]=buffer[8+3];
	_load[4]=buffer[8+4];
	_load[5]=buffer[8+5];
	_load[6]=buffer[8+6];
	_load[7]=buffer[8+7];
	_load[8]=buffer[8+8];
	_load[9]=buffer[8+9];
	_load[10]=buffer[8+10];
	_load[11]=buffer[8+11];
	_load[12]=buffer[8+12];
	_load[13]=buffer[8+13];
	return _load; 
	// noch ungetested wie die Rückgave erfolgen muss
	//return _load.ToString(_load[0]+_load[1]+_load[2]+_load[3]+_load[4]+_load[5]+_load[6]+_load[7]+_load[8]+_load[9]+_load[10]+_load[11]+_load[12]+_load[13]);
	//return str(_load);
}