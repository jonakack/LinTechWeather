

# **Nästa steg för att färdigställa LinTechWeather**

## **NULÄGE - Vad du har:**
✅ **TCP-lager fungerar**: TCPClient och TCPServer kan skicka/ta emot rå data  
✅ **Grundstruktur finns**: HTTPServer har onAccept callback-system  
✅ **Datastrukturer definierade**: httpresponse med headers och body  
⚠️ **HTTP-lager ofärdigt**: Inga funktioner för att faktiskt hantera HTTP  
⚠️ **Ingen väderlogik**: Ingen integration med väder-API  

---

## **STEG 1: Fixa HTTPServer_Init bug**
**Problem**: `HTTPServer_Init` anropar `TCPServer_Initiate` men glömmer skicka `context`  
**Vad som behövs**: Lägg till `_Server` som sista parameter så callback får tillgång till HTTPServer

---

## **STEG 2: Implementera HTTP Request Parsing (Server-sida)**
**Syfte**: Servern måste kunna läsa och förstå HTTP-requests från klienter  

**Vad som behövs i HTTPServer.c eller HTTPConnection.c:**
- **Parse request line**: Extrahera metod (GET/POST), URL-path (`/weather?city=Stockholm`), HTTP-version
- **Parse headers**: Läs Content-Length, Content-Type, Host, etc.
- **Parse query parameters**: Extrahera `city=Stockholm` från URL:en
- **Parse body**: Om POST-request, läs request body

**Input**: Rå text från socket (`"GET /weather?city=Stockholm HTTP/1.1\r\nHost: localhost\r\n\r\n"`)  
**Output**: Strukturerad data (metod, path, parametrar, headers)

---

## **STEG 3: Implementera HTTP Response Building (Server-sida)**
**Syfte**: Servern måste kunna bygga och skicka korrekta HTTP-responses  

**Vad som behövs:**
- **Funktion för att bygga response-string**: Ta httpresponse-struct och konvertera till formaterad text
- **Status line**: `"HTTP/1.1 200 OK\r\n"`
- **Headers**: `"Content-Type: application/json\r\nContent-Length: 123\r\n"`
- **Body**: Själva väderdata i JSON
- **Skicka till socket**: Använd TCPClient_Write för att skicka till klienten

**Input**: httpresponse struct med data  
**Output**: Rå HTTP-text som skickas via TCP

---

## **STEG 4: Implementera HTTPServer_OnAccept logik**
**Syfte**: När klient ansluter, läs request, processera, skicka svar  

**Vad som behövs:**
- Läs data från `_Socketfd` med recv/TCPClient_Read
- Anropa parsing-funktionen från Steg 2
- Bestäm vad som ska göras baserat på URL:
  - `/weather?city=X` → Hämta väder för stad X
  - `/` → Skicka välkomstmeddelande
  - Annat → Skicka 404 Not Found
- Bygg HTTP-response
- Skicka tillbaka med send/TCPClient_Write

---

## **STEG 5: Implementera HTTPClient för väder-API (Klient-sida)**
**Syfte**: Din server måste kunna hämta väderdata från externa API:er (t.ex. OpenWeatherMap, SMHI)  

**Vad som behövs i HTTPClient.c:**
- **HTTP GET-request builder**: Skapa korrekt formaterad GET-request
  - Exempel: `"GET /api/weather?city=Stockholm HTTP/1.1\r\nHost: api.weather.com\r\n\r\n"`
- **Funktion för att göra request**:
  1. Anslut till API med TCPClient_Connect
  2. Skicka GET-request med TCPClient_Write
  3. Ta emot svar med TCPClient_Read
  4. Koppla ner
- **Parse HTTP-response**: Extrahera status code, headers, body från API:ns svar

**Input**: (host, path, query-parametrar)  
**Output**: HTTP response body (oftast JSON)

---

## **STEG 6: Implementera Väderlogik**
**Syfte**: Faktiskt hämta, lagra och servera väderdata  

**Vad som behövs:**
- **Välj väder-API**: SMHI (svenskt, gratis), OpenWeatherMap, eller annat
- **Studera API-dokumentation**: Vilken URL? Vilka parametrar? Vilket format på svar?
- **Implementera API-anrop**: Använd HTTPClient från Steg 5
- **Parse JSON-svar**: Använd ett JSON-bibliotek (cJSON, jsmn) ELLER skriv enkel string-parsing
- **Cache-hantering**: Spara väderdata i `cache/cache.txt` för att undvika för många API-anrop
- **Error handling**: Vad händer om staden inte finns? Om API är nere?

---

## **STEG 7: Integrera Väderlogik i HTTPServer**
**Syfte**: Koppla ihop server, klient och väderlogik  

**Vad som behövs:**
- I `HTTPServer_OnAccept`: När du fått en request för `/weather?city=X`
  1. Kolla cache först: Finns färsk data för X?
  2. Om inte: Använd HTTPClient för att hämta från väder-API
  3. Spara i cache
  4. Formatera som JSON eller HTML
  5. Skicka tillbaka i HTTP-response

---

## **STEG 8: Uppdatera Client main.c**
**Syfte**: Klient-applikationen ska fråga din server om väder  

**Vad som behövs:**
- **Ta emot kommandoradsargument**: `./weather_client Stockholm`
- **Bygg HTTP GET-request**: `/weather?city=Stockholm`
- **Skicka till din lokala server** (localhost:8080)
- **Ta emot HTTP-response**
- **Parse och visa väderdata** snyggt för användaren

**Just nu**: Klienten skickar bara "Hello from client"  
**Mål**: Klienten frågar efter väder och visar resultat

---

## **STEG 9: Uppdatera Server main.c**
**Syfte**: Servern ska använda HTTPServer istället för rå TCPServer  

**Vad som behövs:**
- Byt från `TCPServer` till `HTTPServer`
- Anropa `HTTPServer_Init` istället för `TCPServer_Initiate`
- Låt HTTPServer_OnAccept hantera all HTTP-logik
- Kanske lägg till konfiguration (port, API-nyckel, etc.)

---

## **STEG 10: Felhantering och Förbättringar**
**Syfte**: Gör programmet robust och användarvänligt  

**Vad som behövs:**
- **Timeout-hantering**: Vad händer om väder-API inte svarar?
- **Logging**: Logga alla requests till fil
- **Felmeddelanden**: Tydliga felmeddelanden till användaren
- **Validering**: Kontrollera att städer är giltiga
- **Dokumentation**: README med instruktioner

---

## **STEG 11: HTTPConnection - Om det behövs**
**Beslut**: HTTPConnection.c/h är tom - behövs den?  

**Möjliga användningar:**
- **Persistent connections**: Hålla HTTP-anslutningar öppna mellan requests
- **Connection pooling**: Återanvända TCP-anslutningar till väder-API
- **State management**: Spara information om pågående HTTP-anslutningar

**Beslut du måste ta**: Behövs detta för ditt projekt? Annars kan du skippa den filen.

---

## **PRIORITERAD ORDNING:**

1. **Steg 1** (kritisk bugg)
2. **Steg 2 + 3** (HTTP-grunden)
3. **Steg 4** (serverns hjärna)
4. **Steg 5** (kunna prata med väder-API)
5. **Steg 6** (väderlogik)
6. **Steg 7** (sätt ihop allt)
7. **Steg 8 + 9** (uppdatera main-filerna)
8. **Steg 10** (polish)

---

Vill du börja med något specifikt steg? Eller vill du ha mer detaljer om något?