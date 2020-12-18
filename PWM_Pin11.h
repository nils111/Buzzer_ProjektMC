/*
  
Autor:    Gruppe 1
Datum:    26.11.2020
Funktion: Deklaration aller in der PWM_PIN11.cpp-Datei verwendeten Funktionen, inklusive Geter- und Seter-Funktionen

*/
void initPWM();
void setPWM(int frequenz, int tastbreite);
void resetPWM();
int Grenzwert (int f);
int Schwellenwert (int a, int t);
