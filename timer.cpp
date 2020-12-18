/*
  
Autor:    Gruppe 1
Datum:    03.12.2020
Funktion: Farad Tacko

*/
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// static Variablen sind nicht in anderen Datein nicht sichtbar
// volatile damit zugriff zu jeder Zeit erfolgen kann, da diese im Interrupt gebraucht werden!!
static volatile int intFlagLaeuftDerTimer3=0;  //Zeigt mit 1 an, dass der Timer0 noch läuft. Darauf wartet Main, bevor neuer Durchlauf beginnt

static volatile int intVerlangsamungsZaehler=0;

static int intVerlangsamungsfaktorTimer3=1;


void InitTimer3(int a)
{
  //////////////////////////////////////////////////////////////////////////////
  //Initialisierung des allgemeinen 16bit Timer3 für Zeitbezug main-Schleife/////
  //////////////////////////////////////////////////////////////////////////////

  //Deaktivierung sämtlicher Ausgabepins für PWM, da reine Zeitfunktion, Konfigurationsregister A fuer Timer 3
  TCCR3A= 0;
  //Timer3 auf CTC-Mode: Nach erreichen des im Registers OCR3A gespeicherten Wertes, löst der Timer den Interrupt
  //TIMER3_COMPA_vect aus
  //Prescalar von 8 wird durch CS31 gewaehlt.. dabei 3 fuer Timer 3 und 1 fuer die Einstellung
  TCCR3B= (1 << WGM32)|(1 << CS31);
  //Aktivieren des Interrupts für den Vergleichswert A
  TIMSK3= (1 << OCIE3A);// Interrupt Timer aktivieren
  
  //Teilen des Prozessorstakts
  //CS31 auf 1, restliche CSxy auf 0 => Prescaling 8=> 16Mhz/8=2Mhz Takt
 // TCCR3B|= (1 << CS31);
  
  //Statt bis zum Maximalwert der 16bit zu zählen, wird bis OCR3A mit 1999 gezählt. Von 0 an sind dies 2000 Schritte.
  //f_timer=2Mhz/2000=1kHz => Nach jedem Interrupt ist eine Millisekunde vergangen
  OCR3A = 1999;
  
  //Zaehlwerk des Timer0 auf 0 setzen
  TCNT3  = 0;
  
  intVerlangsamungsfaktorTimer3=a;
  setLaeuftDerTimer3(1);
}

//Wird im Abstand von einer 1/10-Millisekunde ausgeführt, da der Intterrpt beim ablaufen des Timers aufgerudfen wird
ISR(TIMER3_COMPA_vect)
{ 
  // Wenn laenger als 0,1 ms gewartet wird, gibt timer alle 0,1 ms den interrupt aber nur alle zb 10 interrrupts wird die Flag intFlagLaeuftDerTimer3 auf 0 gesetzt
  intVerlangsamungsZaehler++;
  if(intVerlangsamungsZaehler==intVerlangsamungsfaktorTimer3)
  {
  intFlagLaeuftDerTimer3=0;  
  intVerlangsamungsZaehler=0;
  }
}

int getLaeuftDerTimer3(void)
{
  return intFlagLaeuftDerTimer3;
}

void setLaeuftDerTimer3(int a)
{
  intFlagLaeuftDerTimer3=a;
}

void setVerlangsamungsfaktorTimer3(int a)
{
  intVerlangsamungsfaktorTimer3=a;
}
