/*
 * thermistor_misc.c
 *
 *  Created on: Feb 11, 2021
 *      Author: Palotas Gergely
 */

#include "thermistor_misc.h"
#include "stm32f4xx_hal.h"
#include "math.h"
#include "float.h"
#include "string.h"
#include "stdio.h"
#include "OLED.h"

extern OLED_t SSD1306;

//calculate the resistance of the thermistor from the conversion result
float CalculateImpedance(uint32_t conversion_result)
{
	//reference voltage
	const float VDD = 3.3;

	//reference resistor
	const float REFERENCE_IMPEDANCE = 10000;

	//calculate the ADC input voltage
	float input_voltage = VDD * (float)conversion_result / 4095.0;

	//calculate the resistance of the thermistor
	float thermistor_impedance = 0.0;
	if (input_voltage == 0) thermistor_impedance = FLT_MAX;
	else thermistor_impedance = (VDD / input_voltage - 1) * REFERENCE_IMPEDANCE;

	return (thermistor_impedance);
}

//calculate the temperature from the conversion result
float CalculateTemperature(uint32_t conversion_result)
{
	//0 °C based on Kelvin scale
	const float ZERO_IN_KELVIN = 273.15;

	//Steinhart-Hart coefficients
	const float A = 1.135824991E-3f;
	const float B = 2.328905183E-4f;
	const float C = 0.9367585770E-7f;

	//calculate the resistance of the thermistor
	float thermistor_impedance = CalculateImpedance(conversion_result);

	//calculate the temperature with the Steinhart-Hart equations
	float reciprocal_temp = A + B*log(thermistor_impedance) + C* pow(log(thermistor_impedance),3);
	float temp = pow(reciprocal_temp, -1);

	return (temp-ZERO_IN_KELVIN);
}

//Display text
void DisplayText(char* text_first_line, char* text_second_line, int unit)
{
	//clear the previous display
	OLED_Fill(Black);

	//write text to the first line
	OLED_SetCursor(0, 0);
	OLED_WriteString(text_first_line, Font_11x18, White);

	//write text to the second line
	OLED_SetCursor(0, 36);
	OLED_WriteString(text_second_line, Font_11x18, White);

	//write the actual unit
	switch (unit)
	{
	case Celsius:
		OLED_DrawCircle(SSD1306.CurrentX + 4, 38, 2, White);
		OLED_SetCursor(SSD1306.CurrentX + 8, 36);
		OLED_WriteString("C", Font_11x18, White);
		break;
	case NoUnit:
		break;
	default:
		break;
	}

	//update the screen based on the previous settings
	OLED_UpdateScreen();
}

//display text and the associated integer value
void DisplayTextAndInteger(char* text, const int value, int unit)
{
	//convert the integer value to text
	char temp_string[64] = "";
	int ret = snprintf(temp_string, sizeof(temp_string), "%d", value);
	if ((ret < 0) || (ret >= sizeof(temp_string)))
	{
		strcpy(temp_string, "Conv Err");
	}

	DisplayText(text, temp_string, unit);
}

//display text and the associated float value
void DisplayTextAndFloat(char* text, const float value, int unit)
{
	//convert the float value to text with one decimal place
	char temp_string[64] = "";
	int ret = snprintf(temp_string, sizeof(temp_string), "%.1f", value);
	if ((ret < 0) || (ret >= sizeof(temp_string)))
	{
		strcpy(temp_string, "Conv Err");
	}

	DisplayText(text, temp_string, unit);
}
