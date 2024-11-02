//
// Created by Sunny on 24-11-2.
//

#include "imu.h"

#include "spi.h"

void BMI088TransmitByte(uint8_t txData)
{
	HAL_SPI_Transmit(&hspi1, &txData, 1, HAL_MAX_DELAY);
	// while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
}

void BMI088ReceiveByte(uint8_t* rxData)
{
	HAL_SPI_Receive(&hspi1, rxData, 1, HAL_MAX_DELAY);
}

void BMI088WriteSingleByte(GPIO_TypeDef* ssPort, uint8_t ssPin, uint8_t reg, uint8_t txData)
{
	HAL_GPIO_WritePin(ssPort, ssPin, GPIO_PIN_RESET);
	BMI088TransmitByte(reg | 0x00);
	BMI088TransmitByte(txData);
	HAL_GPIO_WritePin(ssPort, ssPin, GPIO_PIN_SET);
}

void BMI088ReadSingleByte(GPIO_TypeDef* ssPort, uint8_t ssPin, uint8_t reg, uint8_t &rxData)
{
	HAL_GPIO_WritePin(ssPort, ssPin, GPIO_PIN_RESET);
	BMI088TransmitByte(reg | 0x80);
	BMI088ReceiveByte(&rxData);
	HAL_GPIO_WritePin(ssPort, ssPin, GPIO_PIN_SET);
}

void BMI088ReadMultipleByte(GPIO_TypeDef* ssPort, uint8_t ssPin, uint8_t reg, uint8_t* rxData, uint8_t length)
{
	HAL_GPIO_WritePin(ssPort, ssPin, GPIO_PIN_RESET);
	BMI088TransmitByte(reg | 0x80);
	for (uint8_t i = 0; i < length; i++)
	{
		BMI088ReceiveByte( rxData + i );
	}
	HAL_GPIO_WritePin(ssPort, ssPin, GPIO_PIN_SET);
}

float linearMapping(int32_t in, int32_t inMin, int32_t inMax, float outMin, float outMax)
{
	return (float)(in-inMin)*(outMax-outMin)/(float)(inMax-inMin)+outMin;
}

imuAccelTypedef imuAccel;
uint8_t accData[6];

void BMI088ReadAccel()
{
	BMI088ReadMultipleByte(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, RATE_DATA_START_REG, accData, 6);
	int16_t accelXInt16 = (int16_t)(accData[0] << 8 | accData[1]);
	int16_t accelYInt16 = (int16_t)(accData[2] << 8 | accData[3]);
	int16_t accelZInt16 = (int16_t)(accData[4] << 8 | accData[5]);
	imuAccel.accelX = linearMapping(accelXInt16, -32767, 32767, -6, 6);
	imuAccel.accelY = linearMapping(accelYInt16, -32767, 32767, -6, 6);
	imuAccel.accelZ = linearMapping(accelZInt16, -32767, 32767, -6, 6);
}

imuGyroTypedef imuGyro;
uint8_t rateData[6];

void BMI088ReadGyro()
{
	BMI088ReadMultipleByte(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, RATE_DATA_START_REG, rateData, 6);
	int16_t rateXInt16 = (int16_t)(rateData[0] << 8 | rateData[1]);
	int16_t rateYInt16 = (int16_t)(rateData[2] << 8 | rateData[3]);
	int16_t rateZInt16 = (int16_t)(rateData[4] << 8 | rateData[5]);
	imuGyro.rateX = linearMapping(rateXInt16, -32767, 32767, -2000, 2000);
	imuGyro.rateY = linearMapping(rateYInt16, -32767, 32767, -2000, 2000);
	imuGyro.rateZ = linearMapping(rateZInt16, -32767, 32767, -2000, 2000);
}

void BMI088Init()
{
	BMI088WriteSingleByte(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, ACC_RANGE_REG, ACC_RANGE_6G);
}