/*
  
Autor:    Gruppe 1
Datum:    26.11.2020
Funktion: Melodie vom Lied "Alle meine Entchen" mittels Tastendruck

Wenn 0 an digitalem Eingangspin liegt, wird Song abgespielt bzw. gestoppt

*/

// Einbinden der nötigen Datein
#include "PWM_Pin11.h"
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Frequenzen für Töne C bis A
int A = 440;
int B = 700;
int C = 262;
int D = 294;
int E = 330;
int F = 350;
int G = 392;


// Hilfsvariable um Funktion zu gewähren
int FlagB = 0;    //Flags verschiedene Eingangsports
int FlagL = 0;
int altB = 0;
int altL = 0;
int Flag1 = 1;    //Flags verschiedene Meldodien und Eingänge
int Flag2 = 1;
int Flag3 = 1;
int PinauslesenB = 0;
int PinauslesenL = 0;
int Zeitpunkt = 0;

// Hilfsvariable um Überlast zu kontrollieren
int bFlagUeberlast = 0;

// Setup treffen
void setup()
{
  // Initialiesierung vom PWM-Signal
  initPWM();
  
  // Initialisierung Timer, alle 10 ms Interrupt
  InitTimer3(10);
  
  // Initialisierung serielle Schnittstelle
  Serial.begin(9600);
  //Serial.println();

  // Setzten des Eingangspin 53 des Arduino-Boards
  DDRB &= (0<<PB0);
  //Pullup aktivieren
  PORTB |= (1<<PB0); 

  // Setzten des Eingangspin 49 des Arduino-Boards
  DDRL &= (0<<PL0);
  //Pullup aktivieren
  PORTL |= (1<<PL0); 

  // Setzten des Eingangspin 45 des Arduino-Boards
  DDRL &= (0<<PL4);
  //Pullup aktivieren
  PORTL |= (1<<PL4); 
}

// Schleife
void loop()
{

  // Auslesen des aktuellen Zustands des Ports B
  PinauslesenB = ~(PINB);
  PinauslesenL = ~(PINL);

  // Wenn unterschied zwischen altem und neuem Zustand UND aktueller Zustand der Pins auf High= Flag an dem Bit = 1
  FlagB = (altB^PinauslesenB) & PinauslesenB;
  
  FlagL = (altL^PinauslesenL) & PinauslesenL;

  // Abspeichern des alten Zustands des Port B für die nächste Schleife
  altB = PinauslesenB;
  altL = PinauslesenL;

  // Wenn positive Flanke an Pin 0 von Port B (53 auf Arduino) erkannt
  if(FlagB & (1<<PB0))
  {
    // Verzweigung in Abhängigkeit zu voherigem Zustand
    // Wenn vorheriger Zustand gleich 1 und erneut Taster gedrückt, Rücksetzten
    // Bei jeder positive Flange wird der Wert Flag getoggelt
    if(Flag1 == 0)
    {
      // Rücksetzten der Flag und des PWM-Siganals
      Flag3 = 1;
      Flag2 = 1;
      Flag1 = 1;
      resetPWM();
      // Rücksetzten des Zeitpunktes, sodass bei erneutem Start das Lied von vorne beginnt
      Zeitpunkt = 0;
    }

    // Wenn vorheriger Zustand gleich 0 dann setzten von Flag auf 1, toggeln
    else
    {
      Flag1 = 0;
      Flag2 = 1;
      Flag3 = 1;
      Zeitpunkt=0;
    }
  }

  // Wenn positive Flanke an Pin 0 von Port L (49 auf Arduino) erkannt
  if(FlagL & (1<<PL0))
  {
    // Verzweigung in Abhängigkeit zu voherigem Zustand
    // Wenn vorheriger Zustand gleich 1 und erneut Taster gedrückt, Rücksetzten
    // Bei jeder positive Flange wird der Wert Flag getoggelt
    if(Flag2 == 0)
    {
      // Rücksetzten der Flag und des PWM-Siganals
      Flag3 = 1;
      Flag2 = 1;
      Flag1 = 1;
      resetPWM();
      // Rücksetzten des Zeitpunktes, sodass bei erneutem Start das Lied von vorne beginnt
      Zeitpunkt = 0;
    }

    // Wenn vorheriger Zustand gleich 0 dann setzten von Flag auf 1, toggeln
    else
    {
      Flag1 = 1;
      Flag2 = 0;
      Flag3 = 1;
      Zeitpunkt=0;
    }
  }

  // Wenn positive Flanke an Pin 4 von Port L (45 auf Arduino) erkannt
  if(FlagL & (1<<PL4))
  {
    // Verzweigung in Abhängigkeit zu voherigem Zustand
    // Wenn vorheriger Zustand gleich 1 und erneut Taster gedrückt, Rücksetzten
    // Bei jeder positive Flange wird der Wert Flag getoggelt
    if(Flag3 == 0)
    {
      // Rücksetzten der Flag und des PWM-Siganals
      Flag3 = 1;
      Flag2 = 1;
      Flag1 = 1;
      resetPWM();
      // Rücksetzten des Zeitpunktes, sodass bei erneutem Start das Lied von vorne beginnt
      Zeitpunkt = 0;
    }

    // Wenn vorheriger Zustand gleich 0 dann setzten von Flag auf 1, toggeln
    else
    {
      Flag1 = 1;
      Flag2 = 1;
      Flag3 = 0;
      Zeitpunkt=0;
    }
  }


  // Wenn Flag1 = 0 wurde der Taster ein mal gedrückt
  if(Flag1==0)
  {

    // Wenn Melodie noch nicht vollständig abgespielt
    if(Zeitpunkt<=1000)
    {
      // Es wird der Funktion Melodie der aktuelle Zeitpunkt übergeben, in Abhängigkeit zu diesem wird der zu spielende Ton bestimmt
      Melodie1(Zeitpunkt);
      // Hochzählen vom Zeitpunkt. Beim Start == 0, nach jeder Schleife die immer 10ms dauert wird er um eins hochgezählt. 
      // BSP: Zeitpunkt 350 = 3.5 s nach Start der Melodie 
      Zeitpunkt++;
    }

    // Melodie vollständig gespielt
    else
    {
      // Rücksetzten des Zeitpunktes, sodass nächster Start am Anfang des Lieds
      Zeitpunkt = 0;
    }
  }

  // Wenn Flag2 = 0 wurde der Taster ein mal gedrückt
  if(Flag2==0)
  {

    // Wenn Melodie noch nicht vollständig abgespielt
    if(Zeitpunkt<=1000)
    {
      // Es wird der Funktion Melodie der aktuelle Zeitpunkt übergeben, in Abhängigkeit zu diesem wird der zu spielende Ton bestimmt
      Melodie2(Zeitpunkt);
      // Hochzählen vom Zeitpunkt. Beim Start == 0, nach jeder Schleife die immer 10ms dauert wird er um eins hochgezählt. 
      // BSP: Zeitpunkt 350 = 3.5 s nach Start der Melodie 
      Zeitpunkt++;
    }

    // Melodie vollständig gespielt
    else
    {
      // Rücksetzten des Zeitpunktes, sodass nächster Start am Anfang des Lieds
      Zeitpunkt = 0;
    }
  }
  
  // Wenn Flag3 = 0 wurde der Taster ein mal gedrückt
  if(Flag3==0)
  {

    // Wenn Melodie noch nicht vollständig abgespielt
    if(Zeitpunkt<=1000)
    {
      // Es wird der Funktion Melodie der aktuelle Zeitpunkt übergeben, in Abhängigkeit zu diesem wird der zu spielende Ton bestimmt
      Melodie3(Zeitpunkt);
      // Hochzählen vom Zeitpunkt. Beim Start == 0, nach jeder Schleife die immer 10ms dauert wird er um eins hochgezählt. 
      // BSP: Zeitpunkt 350 = 3.5 s nach Start der Melodie 
      Zeitpunkt++;
    }

    // Melodie vollständig gespielt
    else
    {
      // Rücksetzten des Zeitpunktes, sodass nächster Start am Anfang des Lieds
      Zeitpunkt = 0;
    }
  }

  // Ermitteln ob Überlast vorhanden
  if (getLaeuftDerTimer3() == 0)
    {
      bFlagUeberlast = 1;
    }
  else
    {
      bFlagUeberlast = 0;
    }


   //Auf Timer3 warten und damit Durchlaufzeit der while(1)-Schleife auf 10ms zu fixieren
  while (getLaeuftDerTimer3() == 1)
    {
    }

   //Zurücksetzen des Flags. Wird nach Ablauf der Zeit des Timer3 erneut gesetzt
  setLaeuftDerTimer3(1);

  // Kontrolle der Überlast

}

// Melodie von "Alle meine Entchen" 
void Melodie1(int i)
{

  // Im Abhängigkeit zu dem jeweiligen Zeitpunkt kann der zu spielende Ton bestimmt werden
  // Alle passenden Frequenzen als Integer definiert
  if(i>=0 && i<25 || i>=900 && i<1000)
  {
    Ton(C);
  }
  else if(i>=25 && i<50 || i>=25 && i<50 || i>=800 && i<900)
  {
    Ton(D);
  }
  else if(i>=50 && i<75 || i>=700 && i<800)
  {
    Ton(E);
  }
  else if(i>=75 && i<100 || i>=600 && i<700)
  {
    Ton(F);
  }
  else if(i>=100 && i<200 || i>=300 && i<400 || i>=500 && i<600)
  {
    Ton(G);
  }
  else if(i>=200 && i<300 || i>=400 && i<500)
  {
    Ton(A);
  }

  // Wenn die 10 Sekunden Melodie vorbei
  else
  {
    // Rücksetzen der Flag und des PWMs, sodass kein Ton mehr gespielt
    resetPWM();
    // Es muss erneut der Taster gedrückt werden, um eine Melodie abzuspeielen
    Flag1 = 1;
    Flag2 = 1;
    Flag3 = 1;
  }
}

void Melodie2(int i)
{

  // Im Abhängigkeit zu dem jeweiligen Zeitpunkt kann der zu spielende Ton bestimmt werden
  // Alle passenden Frequenzen als Integer definiert
  if(i>=0 && i<150 || i>=300 && i<450 || i>=600 && i<750)
  {
    Ton(C);
  }
  else if(i>=150 && i<300 || i>=450 && i<600 || i>=750 && i<1000)
  {
    Ton(G);
  }
  // Wenn die 10 Sekunden Melodie vorbei
  else
  {
    // Rücksetzen der Flag und des PWMs, sodass kein Ton mehr gespielt
    resetPWM();
    // Es muss erneut der Taster gedrückt werden, um eine Melodie abzuspeielen
    Flag1 = 1;
    Flag2 = 1;
    Flag3 = 1;
  }
}

void Melodie3(int i)
{

  // Im Abhängigkeit zu dem jeweiligen Zeitpunkt kann der zu spielende Ton bestimmt werden
  // Alle passenden Frequenzen als Integer definiert
   if(i>=0 && i<1000)
   {
    Ton(A);
   }
  // Wenn die 10 Sekunden Melodie vorbei
  else
  {
    // Rücksetzen der Flag und des PWMs, sodass kein Ton mehr gespielt
    resetPWM();
    // Es muss erneut der Taster gedrückt werden, um eine Melodie abzuspeielen
    Flag1 = 1;
    Flag2 = 1;
    Flag3 = 1;
  }
}


// Funktion um Ton zu setzten 
void Ton(int f)
{
  // Ein PWM Signal mit Tastbreite 50% wird auf PIN 11 vom Arduino-Borad mit der Frequenz f erzeugt
  setPWM(f, 50);
}
