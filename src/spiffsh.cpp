#include <Arduino.h>
#include <FS.h>

String getContentType(String filename) 
{
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

File handleFileRead(String path) 
{
    File file;
    if (SPIFFS.exists(path)) 
    {                                                     
        file = SPIFFS.open(path, "r");                         
        return file;                                  
    }
    return file;                                         
}