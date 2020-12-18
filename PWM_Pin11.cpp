/*
  
Autor:    Gruppe 1
Datum:    26.11.2020
Funktion: PWM-Modul für Pin 11

*/

// Einbinden der benötigten Programme
#include "PWM_Pin11.h"
#include <avr/io.h>
#include <avr/interrupt.h>


// Erste initialiierung des PWM 
void initPWM()
{

  // Pin 5 an Port B als Ausgang für PWM gewählt (PIN 11 an Arduino-Board)
  DDRB |= (1<<PB5);
  
  //WGM1n für n = 1 bis 3 setzt FastPWM ein mit ICR1 als Höchstwert
  TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
  
  //CS Prescaler für Takt CS12 auf 1 und Rest auf 0 bedeutet Teiler gleich 256
  TCCR1B = (1<<WGM13)|(1<<WGM12) | (1<<CS12);
  
  //COM1A1 und COM1B1 aktivieren den PWM-Ausgang für die Werte OCR1A und OCR1B
  //Invertiertender Modus. Aus für Counter unten. An ab OCR1x.
}

// Erneutes Setzten von dem Ausgang mit bestimmeter Frequenz und Tastbreite
void setPWM(int frequenz, int tastbreite)
{
  // Deklaration von Pin 5 von Port B als Ausgang
  DDRB |= (1<<PB5);
  
  // Setzten des oberen Grenzwertes über Funktion Grenzwert()
  ICR1   = Grenzwert(frequenz);        

  // Setzten des Schwellwertes für An und Aus-Breite vom PWM Signal
  OCR1A = Schwellenwert(ICR1, tastbreite);         
}

// Rückstezten des PWMS 
void resetPWM()
{

  // Pin 5 von Port B wird nicht mehr als Ausgang deklariert
  DDRB &= ~(1<<PB5);

  // Grenz- und Schwellwerte auf 0 zurücksetzten
  ICR1 = 0;
  OCR1A = 0;
}


// Brechnung des Grenzwerts in Abhängigkeit zur gewünschten Frequenz
int Grenzwert (int f)
{
  // Mit Presclara von 256 über CS12 gewählt 16MHz/(256*f) = 62500Hz/f
  return (62500/f);
}

// Schwellwert in Abhängigleit zum Maximalwert und der Tastbreite
int Schwellenwert (int a, int t)
{
  // Überprüfen ob Maximalwert restlos durch 2 teilbar ( Überprüfung ob Zahl gerade)
  if(a%2 == 0)
  {
    // Berechnung der Schwelle über zb:  PWM*Grenzwert-1 -->  50%*Grenzwert-1 --> 50/100*Grenzwert-1
    return ((t*a/100)-1);
  }
  else
  {
    // Wenn Wert für Grenzwert ungerade, wird vor Berechnug eine 1 addiert
    return ((t*(a+1)/100)-1);
  }
}
