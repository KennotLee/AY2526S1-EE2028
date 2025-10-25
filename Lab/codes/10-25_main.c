  /******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * (c) EE2028 Teaching Team
  ******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01_accelero.h"
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01_tsensor.h"
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01.h"
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01_hsensor.h"
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01_magneto.h"
#include "stdio.h"
#include "string.h"

static void UART1_Init(void);
extern void initialise_monitor_handles(void);	// for semi-hosting support (printf)
UART_HandleTypeDef huart1;
int main(void)
{
	initialise_monitor_handles(); // for semi-hosting support (printf)
	int seconds_count = 0;
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	UART1_Init();
	;
	/* Peripheral initializations using BSP functions */
	/*BSP_ACCELERO_Init();
	BSP_TSENSOR_Init();
	int count = 0;		// initialise counter

	while (1)
	{
		float accel_data[3];
		int16_t accel_data_i16[3] = { 0 };			// array to store the x, y and z readings.
		BSP_ACCELERO_AccGetXYZ(accel_data_i16);		// read accelerometer
		// the function above returns 16 bit integers which are acceleration in mg (9.8/1000 m/s^2).
		// Converting to float to print the actual acceleration.
		accel_data[0] = (float)accel_data_i16[0] * (9.8/1000.0f);
		accel_data[1] = (float)accel_data_i16[1] * (9.8/1000.0f);
		accel_data[2] = (float)accel_data_i16[2] * (9.8/1000.0f);

		float temp_data;
		temp_data = BSP_TSENSOR_ReadTemp();			// read temperature sensor

		if ((count % 2 == 0)){
			printf("Accel X : %f; Accel Y : %f; Accel Z : %f\n", accel_data[0], accel_data[1], accel_data[2]);
		}

		if ((count % 3 == 0)){
			printf("Temperature : %f\n", temp_data);
		}

		HAL_Delay(500);	// read once every ~0.5second.
		count ++;		// increment counter

	}
*/

	BSP_LED_Init(LED2);
	BSP_HSENSOR_Init();
	BSP_MAGNETO_Init();
	BSP_TSENSOR_Init();
	uint32_t start_tick = HAL_GetTick();

	while (1)
	{

		uint32_t current_tick = HAL_GetTick();
		if ((current_tick - start_tick) >= 1000) {

			seconds_count++;
			char message1[] = "Welcome to EE2028!!!\r\n";
			char message_print[32];
			sprintf(message_print, "%d: %s", seconds_count, message1);
			HAL_UART_Transmit(&huart1, (uint8_t*)message_print, strlen(message_print),0xFFBF); //Sending in normal mode
			uint32_t current_tick = HAL_GetTick();



			BSP_LED_Toggle(LED2);			// toggle LED2
	//--------------------------------------------------------------------------------------------//

			float humidity_data;
			humidity_data = BSP_TSENSOR_ReadTemp();		// read humidity sensor

			printf("Humidity : %f\n", humidity_data);		// print humidity data

			char buf[64];
			int n = snprintf(buf, sizeof(buf), "Temp : %.2f\r\n", humidity_data);
			HAL_UART_Transmit(&huart1, (uint8_t*)buf, (uint16_t)n, HAL_MAX_DELAY);
	//--------------------------------------------------------------------------------------------//

			float magneto_data[3];
			int16_t magneto_data_i16[3] = {0};							// array to store the x, y and z readings.
			BSP_MAGNETO_GetXYZ(magneto_data_i16);						// read magnetometer

			magneto_data[0] = (float)magneto_data_i16[0] * 1000.0f;		//mGauss: * 1000 (find units from data sheet)
			magneto_data[1] = (float)magneto_data_i16[1] * 1000.0f;
			magneto_data[2] = (float)magneto_data_i16[2] * 1000.0f;

			printf("Magneto X : %f; Magneto Y : %f; Magneto Z : %f\n", magneto_data[0], magneto_data[1], magneto_data[2]);

			start_tick = current_tick;
		}



	}
}

static void UART1_Init(void)
{
        /* Pin configuration for UART. BSP_COM_Init() can do this automatically */
        __HAL_RCC_GPIOB_CLK_ENABLE();
         __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;//impt
        GPIO_InitStruct.Pull = GPIO_NOPULL; //impt
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* Configuring UART1 */
        huart1.Instance = USART1;
        huart1.Init.BaudRate = 115200;
        huart1.Init.WordLength = UART_WORDLENGTH_8B;
        huart1.Init.StopBits = UART_STOPBITS_1;
        huart1.Init.Parity = UART_PARITY_NONE;
        huart1.Init.Mode = UART_MODE_TX_RX;
        huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart1.Init.OverSampling = UART_OVERSAMPLING_16;
        huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        if (HAL_UART_Init(&huart1) != HAL_OK)
        {
          while(1);
        }

}
