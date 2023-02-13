/**
  ******************************************************************************
  * @file    main.c 
  * @brief   plik zawierajacy glowne funkcje oraz zadania programu
	* @image html all.png
  ******************************************************************************
  
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "gpio.h"

/**
  ******************************************************************************
  * @brief   ustawienie priorytetu zadania ktore pozwala na "rownloegle" wykonywanie zadan jesli jedno zadanie nic nie robi
  *  - oczywiscie "rownolegle" nie oznacza ze zadania wykonuja sie rownolegle, tylko kolejno po sobie w niewielkich odstepach czasowych	dajac zludne wrazenie rownoleglego wykonywania
	*    ustawienie takiej wartosci pozwala na wykonywanie kolejnych zadan kiedy aktualne zadanie nie wykonuje zadnych zadan
  ******************************************************************************

  */
#define main_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

/**
* @brief   Kolejka z enkodera przenoszaca zmienna enc (wiecej w @link vMain @endlink )



*/
xQueueHandle queue_enkoder;

/**
* @brief   Kolejka z zadania vMain przenoszaca strukture @link display @endlink


*/
xQueueHandle queue_wyswietlacz;



/**
  ******************************************************************************
  * @brief   elementy wchodzace w sklad kolejki pomiedzy zadaniem glownym a wyswietlaczem
  ******************************************************************************
  * @param minute
	* jest to zmienna okreslajaca minuty
	* @param hour
		* jest to zmienna okreslajaca godziny
	* @param time
		* jest to zmienna okreslajaca sekundy
	* @param state
		* jest to zmienna okreslajaca stan w jakim aktualnie sie znajduje (ustawianie/odliczanie)
  ******************************************************************************
  */

typedef struct{
	uint8_t minute;
	uint8_t hour;
	uint8_t time;
	uint8_t state;
}display;
/**
*@brief   Tablica konwersji wartosci BIN na kod wskaznika siedmiosegmentowego
<table>
<caption id="multi_row">Tabela znakow do wyswietlacza </caption>
<tr><th>0xC0  <th>0xF9  <th>0xA4	 <th>0xB0 <th>0x99  <th>0x92  <th>0x82	 <th>0xF8 <th>0x80	 <th>0x90
<tr><td rowspan="1">0 <td>1 <td>2 <td>3    <td>4 <td>5 <td>6 <td>7 <td>8 <td>9 
<table>
 */
const unsigned char seg7[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

/**
*@brief   Ustawienie parametrow zegara systemowego w poprawnego dzialania systemu freeRtos
    
 */
void SystemClock_Config(void);

/**
* @brief   Semafor zalezny od przerwania co 1 sekunde w celu odmierzenia czasu


*/															
xSemaphoreHandle Tim1s;

/**
<table>
<caption id="multi_row">Tabela uzytych wyjsc</caption>
<tr><th>Segment                     <th>Wyjscie       <th>Wyswietlacz		 <th>Wyjscie
<tr><td rowspan="1">pd <td>PD7 <td>LED4 <td> PB14
<tr><td rowspan="1">g <td>PD6 <td>LED3 <td> PB9
<tr><td rowspan="1">f <td>PD5 <td>LED2 <td> PB5
<tr><td rowspan="1">e <td>PD4 <td>LED1 <td> PB1															
<tr><td rowspan="1">d <td>PD7 
<tr><td rowspan="1">c <td>PD2 
<tr><td rowspan="1">b <td>PD1 
<tr><td rowspan="1">a <td>PD0 
</table>

 
    Zmienne wykorzystywane do obslugi wyswietlacza LED

 */
 /**
 *@brief Wartosci poszczegolnych wyswietlaczy
 *@param LED_buf jako wartosc
 */
 
volatile uint8_t LED_buf[4] = {0,0,0,0};

/** 

*@brief Konkretny wyswietlacz		
*@param LED_ptr jako numer wyswietlacza
 */
volatile uint8_t LED_ptr;


/**
* @brief   Zadanie glowne
* @image html vMain.png
* @param state
 * \n

*/


void vMain(void *pvParameters);
void vMain(void *pvParameters)

{


	uint8_t state=0;
	uint16_t i=0;
	uint16_t d=0;
	uint8_t time=0;

	uint8_t minute=2;
	uint8_t hour=0;
	display disp_send;
	
	uint8_t button =0;
	

int8_t enc=0;
	
	while(1){
		


///jesli parametr state jest rowny
{
	switch (state){
		
		case 0:      	 
		/**
*  ******************************************************************************
* * 0 to pobierana jest wartosc +1 lub -1 z kolejki a nastepnie obsluga nastawiania czasu 
		******************************************************************************
 */
			if(xQueueReceive(queue_enkoder, &enc, 10)==pdTRUE){

		if (enc==-1){
							if(hour>0){
						if(hour==9){
							minute=minute;
							hour=hour;
											}
						else{
							if (minute==50){
								minute=0;
								hour++;
							}
							else{
								minute=minute+10;
							}
						}
					}
					else{
					if (minute==59){
					minute=0;
						hour++;}
					else
						minute++;
					}
							
							enc=0;
					
				}
		else if (enc==1){
			
		if(hour==0 && minute==0){
						minute=minute;
						hour=hour;}
					else{
						if (hour==0){
						minute--;
						}
						else{
							if (minute>=10)
							minute=minute-10;
							else{
								if(hour==1&&minute==0){
									hour=0;
									minute=59;
								}
								else{
								hour--;
								minute=50;
							}
							}
											}
						
								}
							
				
		enc=0;
		}
			
		disp_send.time=time;
		disp_send.state=state;
		disp_send.hour=hour;
		disp_send.minute=minute;
		
		xQueueSend(queue_wyswietlacz, &disp_send,( TickType_t ) 40 );}	
		
		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin)) {
			HAL_Delay(40);
			if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin)){
			
			state=1;}
			
			}
		
		break;
						
	
		
		
		case 1:     
			/**
* * 1 to odliczanie czasu pobranego z case 0 i wysylanie na wyswietlacz, w przypadku 00:00 przechodzi do case 2
		******************************************************************************
 */
	
			
			if (xSemaphoreTake(Tim1s, 10)){
		
		if (hour>=0){
			if (minute==0 && time==0){
					hour = hour-1;
					minute = 59;
					time = 59;}
				
				if (minute>=0 && time==0){
								minute = minute-1;
								time = 59;}
				
				else{
					time--;}
				}
		else{
			if (minute==0 && time==0){
				
					minute = 59;
					time = 59;}
				
				if (minute>=0 && time==0){
								minute = minute-1;
								time = 59;}
				else{
					time--;}
			}
	
					
			
				
																						}
		if (minute==0 && time==0 && hour==0){
		state=2;
		}
		disp_send.time=time;
		disp_send.state=state;
		disp_send.hour=hour;
		disp_send.minute=minute;
		
		xQueueSend(queue_wyswietlacz, &disp_send,( TickType_t ) 40 );
		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin)) {
			HAL_Delay(40);
			if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin)){
			
		
			state=0;}
			
			}
		
		break;			
		//wysylamy czas kolejka
	
		
		case 2:			
				/**
* * 2 to nastepuje generowanie sygnalu dzwiekowego i przejscie do case 0
		******************************************************************************
 */
		/**
 *  ******************************************************************************
*@param enc jest to wartosc mowiaca o inkrementacji lub dekrementacji podczas ustawiania czasu

*@param queue_wyswietlacz jest to kolejka opuszczajaca vMain i idaca do @link vDisp @endlink


 */
		for(d=0;d<6;d++){
			for(i=0;i<200;i++){
		HAL_GPIO_TogglePin(BUZZER_GPIO_Port, GPIO_PIN_13);
		HAL_Delay(1);
												}
		HAL_Delay(300);
											}
		state=0;
		break;			
		
		}
	
	
	
	}




}

}




/**
* @brief   Zadanie enkodera
* @image html vEnkoder.png
* @param rotete1 jest to pierwsza pozycja diagramu oraz jest to pierwszy styk enkodera
* @param rotete2 jest to druga pozycja diagramu oraz jest to drugi styk enkodera
* @param state jest to stan w ktorym aktualnie sie znajdujemy
* @param enc jest to zmienna wpisywana do kolejki i idaca do zadania @link vMain @endlink
* \n

*/
/**
*  ******************************************************************************
* * W przypadku przejscia ze stanu 3 do 0 nastepuje wpisanie do zmiennej enc wartosci +1
* * W przypadku przejscia ze stanu 6 do 0 nastepuje wpisanie do zmiennej enc wartosci -1
 */
void vEncoder(void *pvParameters);
void vEncoder(void *pvParameters)
	{
	
	static uint8_t rotate1 = 0; 	//lewo
	static uint8_t rotate2 = 0;		//prawo
	static uint8_t state = 0;			//stan
	int8_t	enc;
	


	
		
	for( ;; )
	{

		
		
	if (GPIO_PIN_SET == HAL_GPIO_ReadPin(SW_EXT2_GPIO_Port, SW_EXT2_Pin)) {
		rotate1=1;	
		HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);}	
	if (GPIO_PIN_SET == HAL_GPIO_ReadPin(SW_EXT_1_GPIO_Port, SW_EXT_1_Pin))	{
		rotate2=1;	
		HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);}
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(SW_EXT2_GPIO_Port, SW_EXT2_Pin)){
		rotate1=0;	
		HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);}		
	if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(SW_EXT_1_GPIO_Port, SW_EXT_1_Pin))	{
		rotate2=0;	
		HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);}
			
			switch (state)
		{
			case 0 : 
				if(rotate1 == 1 && rotate2 ==0)
					state =1;
				else if(rotate1 == 0 && rotate2 ==0)
					state =0;
				else if(rotate1 == 0 && rotate2 ==1)
					state =4;
			break;
		
			case 1 : 
				if(rotate1 == 1 && rotate2 ==1)
					state =2;
				else if(rotate1 == 1 && rotate2 ==0)
					state =1;
				else if(rotate1 == 0 && rotate2 ==0)
					state =0;
			break;
		
			case 2 : 
				if(rotate1 == 1 && rotate2 ==1)
					state =2;
				else if(rotate1 == 1 && rotate2 ==0)
					state =1;
				else if(rotate1 == 0 && rotate2 ==1)
					state =3;
				break;
		
			case 3 : 
				if(rotate1 == 1 && rotate2 ==1)
					state =2;
				else if(rotate1 == 0 && rotate2 ==1)
					state =3;
				else if(rotate1 == 0 && rotate2 ==0)
					{

				enc=1;
				xQueueSend(queue_enkoder, &enc,( TickType_t ) 10 );
						state=0;
					}
			break;
		
		case 4 : 
				if(rotate1 == 0 && rotate2 ==1)
					state =4;
				else if(rotate1 == 1 && rotate2 ==1)
					state =5;
				else if(rotate1 == 0 && rotate2 ==0)
					state =0;
			break;
		
			case 5 : 
				if(rotate1 == 1 && rotate2 ==1)
					state =5;
				else if(rotate1 == 1 && rotate2 ==0)
					state =6;
				else if(rotate1 == 0 && rotate2 ==1)
					state =4;
				break;
		
			case 6 : 
				if(rotate1 == 1 && rotate2 ==1)
					state =5;
				else if(rotate1 == 1 && rotate2 ==0)
					state =6; //zbedne
				else if(rotate1 == 0 && rotate2 ==0)
					{
					enc=-1;
					xQueueSend(queue_enkoder, &enc,( TickType_t ) 10 );
						state=0;
						}
						
			break;
		
				
						
					}
		
		
		//xQueueSend(queue_enkoder, &czas_wysylany,( TickType_t ) 10 );
		

		
				
	}
			}	

	/**
* @brief   Zadanie wyswietlacza
* @param time jest to czas w sekundach
* @param minute jest to czas w minutach
* @param hour jest to czas w godzinach

* \n

*/		
void vDisp(void *pvParameters);
void vDisp(void *pvParameters)
	{
	uint8_t time=0;
	uint8_t minute=0;
	uint8_t hour=0;

		
		
		while(1){
			display disp_send;
			xQueueReceive(queue_wyswietlacz, &disp_send, portMAX_DELAY);
			hour = disp_send.hour;
			minute = disp_send.minute;
			time = disp_send.time;
/**
* @code {.c}
*/
		if (hour==0){
			LED_buf[3] = seg7[(minute %100) /10];
			LED_buf[2] = seg7[minute % 10];
			LED_buf[1] = seg7[(time %100) /10];
			LED_buf[0] = seg7[time % 10];
									}
		else					{
			LED_buf[3] = seg7[0];
			LED_buf[2] = seg7[hour];
			LED_buf[1] = seg7[(minute %100) /10];
			LED_buf[0] = seg7[minute % 10];
			
									}
			
/**
* Mozna zauwazyc sposob zmiany zakresu wyswietlania
* @endcode
*/
									/**
* Mozna zauwazyc sposob zmiany zakresu wyswietlania
*/
			
		}
	}
	
			
	/**
*@brief   Ustawienie parametrow zegara systemowego w celu odliczania 1 sekundy oraz sterowanie wyswietlaczami
    
 */		
void vApplicationTickHook(void)
{
	static uint16_t onoff=0;
	static uint8_t time2ms = 0;
	static uint16_t time1s = 0;
  signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  
  if (++time2ms > 1)
  {
    time2ms = 0;
  
    // obsluga odmierzania czasu 1s
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (time1s++ > 500)
    {
			//HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);
			time1s = 0;
      xSemaphoreGiveFromISR(Tim1s, &xHigherPriorityTaskWoken);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // obsluga wyswietlacza siedmiosegmentowego LED
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
		
		if ((++LED_ptr) > 3) LED_ptr = 0;
    // wylaczenie wyswietlaczy
		
    HAL_GPIO_WritePin(LED_WSW_GPIO_Port, LED_WSW_4_Pin|LED_WSW_3_Pin|LED_WSW_2_Pin
                                        |LED_WSW_1_Pin, GPIO_PIN_RESET);
		
    // ustawienie segmentow
    LED_EXT_GPIO->BSRR = (uint32_t)LED_buf[LED_ptr];
    LED_EXT_GPIO->BSRR = (uint32_t)(~LED_buf[LED_ptr]) << 16;

    // wybor wyswietlacza
		//if(onoff<=400){
    switch (LED_ptr)
    {
			
      case 0: HAL_GPIO_WritePin(LED_WSW_GPIO_Port, LED_WSW_1_Pin, GPIO_PIN_SET);
              break;
      case 1: HAL_GPIO_WritePin(LED_WSW_GPIO_Port, LED_WSW_2_Pin, GPIO_PIN_SET);
              break;
      case 2: HAL_GPIO_WritePin(LED_WSW_GPIO_Port, LED_WSW_3_Pin, GPIO_PIN_SET);
              break;
      case 3: HAL_GPIO_WritePin(LED_WSW_GPIO_Port, LED_WSW_4_Pin, GPIO_PIN_SET);
              break;
    }
}
  
  if (xHigherPriorityTaskWoken == pdTRUE) taskYIELD();
}


/**
* @brief   Ladowanie calego systemu FreeRtos oraz inicjalizacja wykorzystywanych funkcji


*/
int main(void)
{
  
  /** Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /** Configure the system clock */
  SystemClock_Config();
  /** Initialize all configured peripherals */
  MX_GPIO_Init();
	/**  Stworzenie kolejek */
/** * Stworzenie pierwszej kolejki @link queue_enkoder @endlink */
queue_enkoder = xQueueCreate(5,sizeof(int8_t));
	/** * Stworzenie drugiej kolejki @link queue_wyswietlacz @endlink */
queue_wyswietlacz = xQueueCreate(5,sizeof(display));

	/**  Stworzenie semafora */
	/** * Stworzenie semafora @link Tim1s @endlink */
  vSemaphoreCreateBinary(Tim1s);

/**  Stworzenie zadan */
	 /** * Stworzenie pierwszego zadania @link vMain @endlink */
	xTaskCreate(vEncoder, "Tsk2", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
		 /** * Stworzenie drugiego zadania @link vEncoder @endlink */
	xTaskCreate(vMain, "Tsk1", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);
	 /** * Stworzenie trzeciego zadania @link vDisplay @endlink*/
	xTaskCreate(vDisp, "Tsk3", configMINIMAL_STACK_SIZE, NULL, main_TASK_PRIORITY, NULL);

  /** Start scheduler */
  vTaskStartScheduler();
  for(;;);
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  __HAL_RCC_PLLI2S_ENABLE();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
 

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
