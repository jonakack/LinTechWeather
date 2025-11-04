# LinTechWeather API
Under utveckling. 

## Kommande features:
Felkoder, t.ex om stad inte hittas.

Eventuellt en lista på städer om det finns flera städer med samma namn, vi får diskutera den bästa lösningen gemensamt.

# Steg för steg-guide:
Ni anropar denna länk och byter ut `<cityname>` med staden ni vill söka efter:

```
http://localhost:8080/api/v1/geo?city=<cityname>
```

Servern svarar (om namnet är en giltig stad) med t.ex:
```json
{ "city":"cityname","country":"country","lat":"latitude","lon":"longitude" }
```

Ni anropar nästa länk med dessa koordinater:

```
http://localhost:8080/api/v1/weather?lat=<latitude>&lon=<longitude>
```

Servern svarar med t.ex:
```json
{ "tempC":"temperature","description":"weathercode","updatedAt":"dateTimeZ" }
```

## Exempel på svar vid fel:
```json
{ "error": { "code": 404, "message": "city not found" } }
```

## Anropa server via webbläsaren med en av dessa länkar:
http://localhost:8080/api/v1/geo?city=Stockholm

http://localhost:8080/api/v1/weather?lat=59.3293&lon=18.0686

http://localhost:8080/invalidpath


## Eller via en terminal med detta kommando och curl installerat:
curl -l '127.0.0.1:8080/api/v1/geo?city=Stockholm'

curl -l '127.0.0.1:8080/api/v1/weather?lat=59.3293&lon=18.0686'

curl -l '127.0.0.1:8080/api/v1/invalidpath'

## Första Endpointen
### Anropas med t.ex. "/api/v1/geo?city=Stockholm". Då svarar server detta till klient:
{ "city":"Stockholm","country":"SE","lat":59.3293,"lon":18.0686 }

## Andra Endpointen
### Anropas med t.ex. "/api/v1/weather?lat=59.3293&lon=18.0686". Då svarar server detta till klient:
{ "tempC":7.0,"description":"0","updatedAt":"2025-11-02T09:00:00Z" }

# CodeﾠDescription 
### Väderkoden som ges har följande betydelse:
0ﾠClear sky

1, 2, 3ﾠMainly clear, partly cloudy, and overcast

45, 48ﾠFog and depositing rime fog

51, 53, 55ﾠDrizzle: Light, moderate, and dense intensity

56, 57ﾠFreezing Drizzle: Light and dense intensity

61, 63, 65ﾠRain: Slight, moderate and heavy intensity

66, 67ﾠFreezing Rain: Light and heavy intensity

71, 73, 75ﾠSnow fall: Slight, moderate, and heavy intensity

77ﾠSnow grains

80, 81, 82ﾠRain showers: Slight, moderate, and violent

85, 86ﾠSnow showers slight and heavy

95 *ﾠThunderstorm: Slight or moderate

96, 99 *ﾠThunderstorm with slight and heavy hail
