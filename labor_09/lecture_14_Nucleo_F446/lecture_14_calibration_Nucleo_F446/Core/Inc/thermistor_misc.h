#ifndef _THERMISTOR_MISC_H
#define _THERMISTOR_MISC_H

#include "stdint.h"

//possible units
enum Unit
{
	NoUnit = 0,
	Celsius = 1
};

//calculate the temperature from the conversion result
float CalculateTemperature(uint32_t conversion_result);
//Display text
void DisplayText(char* text_first_line, char* text_second_line, int unit);
//display text and the associated integer value
void DisplayTextAndInteger(char* text, const int value, int unit);
//display text and the associated float value
void DisplayTextAndFloat(char* text, const float value, int unit);

#endif  // _THERMISTOR_MISC_H
