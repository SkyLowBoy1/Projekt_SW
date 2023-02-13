Autorzy projektu
* **Marcin Ludyga**
* **Adrian Nowak**

******************************************************************************

Zalozenia projektu



* Nastawianie czasu w zakresie od 1 minuty do 9 godzin przy uzyciu enkodera
* Zmiana czasu 
  * co 1 minute jezeli nie przekroczono godziny
  * co 10 minut jezeli przekroczono godzine
* Wyswietlanie na wyswietlaczu czasu w formacie
  * mm:ss jesli nie przekroczono godziny
  * hh:mm jesli przekroczono godzine
* Zmiana pracy urzadzenie (ustawianie/odliczanie) przy pomocy przycisku
* Odmierzenie czasu sygnalizowane dzwiekowo
******************************************************************************

Pliki projektu

  ******************************************************************************
  *     main.c 
    * vMain() \ref handle "zadanie glowne"
    * vEncoder() \ref handle "zadanie enkodera"
    * vDisp() \ref handle "zadanie wyswietlacza"
  *	   stm32f1xx_hal_timebase_TIM.c
  *     stm32f1xx_it.c

  ******************************************************************************
  

