//
// Created by Sunny on 24-11-2.
//

#include "startup.h"
#include "imu.h"
#include "tim.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim6)
	{
		BMI088ReadAccel();
		BMI088ReadGyro();
	}
}

void startup()
{
	BMI088Init();
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start(&htim10);
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim10, TIM_CHANNEL_1, 50);
	while (1)
	{

	}
}
