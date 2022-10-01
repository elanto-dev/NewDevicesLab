/*
 * A simple example to interface with rdm6300 rfid reader using
 * hardware uart "Serial" instead of the default software uart driver.
 *
 * Will only compile for boards with Serial1 or more:
 * https://www.arduino.cc/reference/en/language/functions/communication/serial/
 *
 * Arad Eizen (https://github.com/arduino12).
 */

#include <rdm6300.h>

#define READ_LED_PIN 13

Rdm6300 rdm6300;

void setup()
{
	Serial.begin(115200);

	pinMode(READ_LED_PIN, OUTPUT);
	digitalWrite(READ_LED_PIN, LOW);

	Serial1.begin(RDM6300_BAUDRATE); // read the doc above, find the RX pin number in the link
	rdm6300.begin(&Serial1);

	Serial.println("\nPlace RFID tag near the rdm6300...");
}

void loop()
{
	/* if non-zero tag_id, update() returns true- a new tag is near! */
	if (rdm6300.update())
		Serial.println(rdm6300.get_tag_id(), HEX);

	digitalWrite(READ_LED_PIN, rdm6300.is_tag_near());

	delay(10);
}
