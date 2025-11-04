# LinTechWeather API
Under utveckling
## Anropa server via webbläsaren med en av dessa länkar:
http://localhost:8080/api/v1/geo?city=Stockholm

http://localhost:8080/api/v1/weather?lat=59.3293&lon=18.0686

http://localhost:8080/invalidpath

Kommer i framtiden inte vara lokalt.

## Eller via en terminal med detta kommando och curl installerat:
curl -l '127.0.0.1:8080/api/v1/geo?city=Stockholm'

curl -l '127.0.0.1:8080/api/v1/weather?lat=59.3293&lon=18.0686'

curl -l '127.0.0.1:8080/api/v1/invalidpath'

## Första Endpoint
### Anropas med t.ex. "/api/v1/geo?city=Stockholm". Då svarar server detta till klient:
{ "city":"Stockholm","country":"SE","lat":59.3293,"lon":18.0686 }

## Andra Endpoint
### Anropas med t.ex. "/api/v1/weather?lat=59.3293&lon=18.0686". Då svarar server detta till klient:
{ "tempC":7.0,"description":"0","updatedAt":"2025-11-02T09:00:00Z" }

# CodeﾠDescription 
### Innehåller en av dessa variabler:
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
