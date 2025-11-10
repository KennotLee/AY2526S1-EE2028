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
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01_psensor.h"
#include "../../Drivers/BSP/B-L4S5I-IOT01/stm32l4s5i_iot01_gyro.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "buzzer.h"
#include "MotionLite.h"
#include <stdbool.h>
#include <stdint.h>
#include "../../Drivers/OLED/ssd1306.h"
#include "../../Drivers/OLED/ssd1306_fonts.h"
#include "DipSwitch.h"
#include "5Switch.h"


static MotionLite motion;
//Buzzer setup





static void UART1_Init(void);
extern void initialise_monitor_handles(void);	// for semi-hosting support (printf)
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void SystemClock_Config(void);
#define NUMOFMODES 2

const int w = 2;
int8_t welcomed = 0; //Unwelcome school
uint32_t lastPressed = 0;
uint32_t lastDoublePressed = 0;
int8_t mode = 0; //Meaning I'm in Sotong Game <3


I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;
float gyro_data[3], accel_data[3], mag_data[3];

int16_t accel_data_i16[3] = {0};
int16_t mag_data_i16[3] = {0};
void get_imu_readings(void) {
	// imu readings
	BSP_GYRO_GetXYZ(gyro_data);	// units of deg/sec
	BSP_ACCELERO_AccGetXYZ(accel_data_i16);	//units of g
	BSP_MAGNETO_GetXYZ(mag_data_i16);	//units of uT (micro Tesla)
	for (int a = 0; a < 3; a++) { //anything done in post
		accel_data[a] = (float)accel_data_i16[a] * (9.8/1000.0f);
		mag_data[a] = (float) mag_data_i16[a];
	}

//	char welcome_print[64];
//	sprintf(welcome_print, "Maggots: %d %d %d \r\n", accel_data[0], accel_data[1], accel_data[2]);
//	HAL_UART_Transmit(&huart1, (uint8_t*)welcome_print, strlen(welcome_print), 0xFFBF);
//	printf("Accel Data: X = %.2f, Y = %.2f, Z = %.2f\r\n", accel_data[0], accel_data[1], accel_data[2]);
//	printf("Gyro Data: X = %.2f, Y = %.2f, Z = %.2f\r\n", gyro_data[0], gyro_data[1], gyro_data[2]);
//	printf("Mag Data: X = %.2f, Y = %.2f, Z = %.2f\r\n", mag_data[0], mag_data[1], mag_data[2]);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == BUTTON_EXTI13_Pin)
	{
		uint32_t nowTick = HAL_GetTick();

		if (lastPressed > 0 && (nowTick - lastPressed) <= 1000) {
//			sprintf(welcome_print, "Expecting change \r\n");
//			HAL_UART_Transmit(&huart1, (uint8_t*)welcome_print, strlen(welcome_print), 0xFFBF);
			welcomed = 0;
			if (mode >= 0) { //filter out stop mode
				mode++;

			}
			if (mode >= w) {
				mode = 0;
			}

		}

		lastPressed = nowTick;

	}
}

#define MAGNETIC_THRESHOLD_LOW 1500
#define MAGNETIC_THRESHOLD_MED 3000
#define MAGNETIC_THRESHOLD_HIGH 5000
#define TEMP_THRESHOLD 35.0f
#define HUMIDITY_THRESHOLD 70.0f
#define PRESSURE_THRESHOLD 1020.0f

#define BUTTON_TIMEOUT_MS 3000
#define ENV_CHECK_INTERVAL_MS 1000
#define DOUBLE_PRESS_WINDOW_MS 500
#define GYROTHRESH 100000.0f
#define ACCELERO_THRESHOLD 15.0



static float Calculate_Magnetic_Magnitude(void)
{
        float magnitude = sqrtf((float)(mag_data[0]*mag_data[0]) +
                                (float)(mag_data[1]*mag_data[1]) +
                                (float)(mag_data[2]*mag_data[2]));
        return magnitude;
}
static int checkMovement(void) {
    float magnitude = sqrtf((float)(accel_data[0]*accel_data[0]) +
                            (float)(accel_data[1]*accel_data[1]) +
                            (float)(accel_data[2]*accel_data[2]));
    if (magnitude > ACCELERO_THRESHOLD) {
    	return 1;
    }
    magnitude = sqrtf((float)(gyro_data[0]*gyro_data[0]) +
					  (float)(gyro_data[1]*gyro_data[1]) +
					  (float)(gyro_data[2]*gyro_data[2]));
    return magnitude > GYROTHRESH;

}


int main(void)
{
	initialise_monitor_handles(); // for semi-hosting support (printf) // used if and only if for testing

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	SystemClock_Config();
	Buzzer_Init();
	UART1_Init();

	BSP_LED_Init(LED2);
	BSP_HSENSOR_Init();
	BSP_MAGNETO_Init();
	BSP_GYRO_Init();
	BSP_ACCELERO_Init();
	BSP_TSENSOR_Init();
	BSP_PSENSOR_Init();
	get_imu_readings();

	MX_GPIO_Init();
	MX_I2C1_Init();
	ssd1306_Init(); //All ze initializations
//	ssd1306_Fill(White);
//	ssd1306_UpdateScreen(); //Test if the OLED works

	//OLED Logic //setting up for Red Light Green lIght snice it's the default gamemdoe
	ssd1306_SetCursor(5,5);

	ssd1306_WriteString("Red Light", Font_7x10, White);
	ssd1306_SetCursor(5,15);
	ssd1306_WriteString("Green Light", Font_7x10, White);
	ssd1306_UpdateScreen();


	motionlite_init(&motion);
	uint32_t start_tick = HAL_GetTick();
	int seconds_count = 0;
	float mag_mag = 0;
	uint8_t tick20 = 0;
	int8_t green = 1;
	int recordTagTick = 0;
	int iframes = 0;
	float accel_offset[3];
	float gyro_offset[3];
	int8_t count10 = 0;
	uint32_t stop_beep = 0;

	while (1) {

		uint32_t current_tick = HAL_GetTick();

		CheckBuzzer(current_tick);
		if ((current_tick - start_tick) >= 50) {
			//checks if a second has
			if (Switch_GetDir() == CENTER){
				ssd1306_Fill(Black);
				ssd1306_UpdateScreen();
				ssd1306_SetCursor(5,5);
				ssd1306_WriteString("Diagnostics", Font_7x10, White);
				ssd1306_UpdateScreen();
				mode = -2;
			}

			tick20++;
			if (tick20 >= 20) {
				seconds_count++;
				tick20 = 0;

			}

			if (mode == 0) {
				if (welcomed == 0) { //default gamemode when on
					char welcome_print[64];
					sprintf(welcome_print, "Entering Red Light Green Light! You are a Player \r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)welcome_print, strlen(welcome_print), 0xFFBF);
					welcomed = 1;

				}

				if (seconds_count % 10 == 0 && tick20 == 0) { //10s logic
					if (green) { //to Red Light
						green = 0;
						ssd1306_Fill(Black);
						ssd1306_UpdateScreen();
						ssd1306_SetCursor(5,5);
						ssd1306_WriteString("Red Light", Font_7x10, White);
						ssd1306_UpdateScreen();
						Buzzer_Beep(2103, current_tick, 500);

					} else { // to Green Light
						ssd1306_Fill(Black);
						ssd1306_UpdateScreen();
						BSP_LED_On(LED2);
						green = 1;
						Buzzer_Beep(2255, current_tick, 500);
						ssd1306_SetCursor(5,5);
						ssd1306_WriteString("Green Light", Font_7x10, White);
						ssd1306_UpdateScreen();
						Buzzer_Beep(2103, current_tick, 500);
					}

	//				char *lightMessage[] = ;
					char lightMessage_print[64];
					sprintf(lightMessage_print, green ? "Green Light\r\n" : "Red Light\r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)lightMessage_print, strlen(lightMessage_print), 0xFFBF);
				}
				if ((seconds_count + 3) % 10 == 0 && (tick20 == 0 || tick20 == 3)) {
					if (green) {
						Buzzer_Beep(2166, current_tick, 80); //beeps when
					}
				}

				if (green) {
					if (seconds_count % 2 == 0 && tick20 == 0) { //When Green Light
						//Temperature
						float temp_data;
						temp_data = BSP_TSENSOR_ReadTemp();
						char tempMessage[32];
						sprintf(tempMessage, "Temp: %.2fC \r\n", temp_data);
						HAL_UART_Transmit(&huart1, (uint8_t*)tempMessage, strlen(tempMessage), 0xFFBF);

//						char dllm[64];
//						sprintf(dllm, "s: %d t20: %d \r\n", seconds_count, tick20);
//						HAL_UART_Transmit(&huart1, (uint8_t*)dllm, strlen(dllm), 0xFFBF);
						//993, 1493


						//Pressure
						float pres_data;
						pres_data = BSP_PSENSOR_ReadPressure();
						char presMessage[32];
						sprintf(presMessage, "Pressure: %.2f \r\n", pres_data);
						HAL_UART_Transmit(&huart1, (uint8_t*)presMessage, strlen(presMessage), 0xFFBF);

						//Humidity
						float humid_data;
						humid_data = BSP_HSENSOR_ReadHumidity();
						char humidMessage[32];
						sprintf(humidMessage, "Humidity: %.2f \r\n", humid_data);
						HAL_UART_Transmit(&huart1, (uint8_t*)humidMessage, strlen(humidMessage), 0xFFBF);

					}
				} else { //When Red Light
					if (tick20 % 10 == 0) { //Logic for every 0.5s
						BSP_LED_Toggle(LED2);
						get_imu_readings();
						if (checkMovement()) {
						        char print[64];
								sprintf(print, "You Moved! You Lose\r\n");
								HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
								Buzzer_Beep(2255, current_tick, 4500);
								mode = -1; //No game
						}





					}

				}

			} else if (mode == 1) { //Catch & run gamemode
				if (welcomed == 0) { //default gamemode when on
					char welcome_print[64];
					sprintf(welcome_print, "Entering Catch & Run. Catch me if you can! \r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)welcome_print, strlen(welcome_print), 0xFFBF);
					welcomed = 1;
					get_imu_readings();
					mag_mag = Calculate_Magnetic_Magnitude();
					ssd1306_Fill(Black);
					ssd1306_UpdateScreen();
					ssd1306_SetCursor(5,5);
					ssd1306_WriteString("Catch & Run", Font_7x10, White);
					ssd1306_UpdateScreen();
					Buzzer_Beep(2103, current_tick, 500);

				}
				char print[64];
				if (recordTagTick > 6) {

					sprintf(print, "You Got Caught! You Lose\r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
					Buzzer_Beep(2255, current_tick, 4500);
					mode = -1; //No game
				}
				if (current_tick - lastPressed < 51) {
					//Escaped
					iframes = 10; //5 seconds to run again!
					sprintf(print, "You Escaped! Keep running\r\n");
					HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
				}


				if (tick20 % 10 == 0){
//					sprintf(print, "Magneto: %f \r\n", mag_mag);
//					HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
					if (iframes > 0){
						iframes --;
					}

					mag_mag = Calculate_Magnetic_Magnitude();

					if(mag_mag >= MAGNETIC_THRESHOLD_HIGH) {
//						sprintf(print, "Magneto: %f \r\n", mag_mag);
//						HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
						if (recordTagTick == 0) {
							sprintf(print, "Enforcer is Nearby! Be careful. \r\n");
							HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
							recordTagTick ++;
						}

						else{
							recordTagTick++;
						}
						BSP_LED_Toggle(LED2);

					} else if (mag_mag >= MAGNETIC_THRESHOLD_MED) {
						recordTagTick = 0;
//						sprintf(print, "Safe, Moderate");
//						HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
						if(tick20 % 4 == 0) {
							BSP_LED_Toggle(LED2);
						}
					} else if (mag_mag >= MAGNETIC_THRESHOLD_LOW) {
//						sprintf(print, "Safe, Low");
//						HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
						recordTagTick = 0;
						if(tick20 % 10 == 0) {
							BSP_LED_Toggle(LED2);
						}
					} else {BSP_LED_Off(LED2);}

					if (tick20 == 0){
						float temp_data;
						temp_data = BSP_TSENSOR_ReadTemp();
						float pres_data;
						pres_data = BSP_PSENSOR_ReadPressure();
						get_imu_readings();
						float humid_data;
						humid_data = BSP_HSENSOR_ReadHumidity();

						if (temp_data > TEMP_THRESHOLD) {
							sprintf(print, "Warning! High Temperature\r\n");
							HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
							Buzzer_Beep(2255, current_tick, 100);
						}

						if (pres_data > PRESSURE_THRESHOLD) {
							sprintf(print, "Warning! High Pressure\r\n");
							HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
							Buzzer_Beep(2255, current_tick, 100);
						}

						if (humid_data > HUMIDITY_THRESHOLD) {
							sprintf(print, "Warning! High Humidity\r\n");
							HAL_UART_Transmit(&huart1, (uint8_t*)print, strlen(print), 0xFFBF);
							Buzzer_Beep(2255, current_tick, 100);
						}

					}


				}



			}

			else if(mode == -1) {
//				return; //Game Over
				if (stop_beep == 0){
					stop_beep = current_tick + 5000;
				}
				if (current_tick >= stop_beep){
					return;
				}
				ssd1306_Fill(Black);
				ssd1306_UpdateScreen();
				ssd1306_SetCursor(5,5);
				ssd1306_WriteString("Game Over!", Font_7x10, White);
				ssd1306_UpdateScreen();


			}
			else if (mode == -2) { // diagnostics mode

				if (tick20 % 10 == 0) {
					get_imu_readings();
					if (count10 >= 10) {

						for (int dva = 0;dva < 3; dva++) {
							gyro_offset[dva] =(float) gyro_offset[dva] / 10.0;
							accel_offset[dva] = (float) accel_offset[dva] / 10.0;
						}
						char tempMessage[128];
						sprintf(tempMessage, "Accel Offset: X:%.5f Y:%.5f Z:%.5f \r\n", accel_offset[0], accel_offset[1], accel_offset[2]);
						HAL_UART_Transmit(&huart1, (uint8_t*)tempMessage, strlen(tempMessage), 0xFFBF);
						sprintf(tempMessage, "Gyro Offset: X:%.5f Y:%.5f Z:%.5f \r\n", gyro_offset[0], gyro_offset[1], gyro_offset[2]);
						HAL_UART_Transmit(&huart1, (uint8_t*)tempMessage, strlen(tempMessage), 0xFFBF);
						return;
					}
					for (int dva = 0;dva < 3; dva++) {
						gyro_offset[dva] += gyro_data[dva];
						accel_offset[dva] += accel_data[dva];
					}
					count10++;
				}
			}
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

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_RCCEx_EnableMSIPLLMode();
}


static void MX_GPIO_Init(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();	// Enable AHB2 Bus for GPIOC

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// Configuration of BUTTON_EXTI13_Pin (GPIO-C Pin-13) as AF,
	GPIO_InitStruct.Pin = BUTTON_EXTI13_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Enable NVIC EXTI line 13
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


static void MX_I2C1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	// Configure GPIO PB8 and PB9
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Configure I2C1
	hi2c1.Instance = I2C1;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.Timing = 0x00C0216C;	//400khz
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	__HAL_RCC_I2C1_CLK_ENABLE();

	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void) {
	printf("Error"); //Only Print Statement
}






