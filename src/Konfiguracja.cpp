#include "Konfiguracja.h"
 
// Date constructor
Konfiguracja::Konfiguracja(String plik)
{
    p_plik = "/" + plik;
}

void Konfiguracja::zapisz(int l, String t)
{
    File f = SPIFFS.open(p_plik, "r");
    String konfig[3];
    
    if (f) 
    { 
        int lp = 0;
        while(f.available()) 
        {
            String s = f.readStringUntil('\n');
            s.replace("\r", "");
            s.replace("\n", "");
            konfig[lp]= s;    

            Serial.println(s);    
            lp++;
        }
        konfig[l] = t;
        f.close(); 
    }

    f = SPIFFS.open(p_plik, "w");
    String p = konfig[0] + "\n" + konfig[1] + "\n" + konfig[2] + "\n";
    Serial.println(p);
    f.print(p);
    f.close(); 
}

String Konfiguracja::czytaj(int l)
{
    File f = SPIFFS.open(p_plik, "r");
    String konfig[3];
    int lp = 0;
    if (f) 
    { 
        while(f.available()) 
        {
            String s = f.readStringUntil('\n');
            s.replace("\r", "");
            s.replace("\n", "");
            konfig[lp]= s;        
            lp++;
        }
        f.close();
    }
    return konfig[l];
}
