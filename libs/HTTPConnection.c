/* Parse request line: Extrahera metod (GET/POST), URL-path (/weather?city=Stockholm), HTTP-version
Parse headers: Läs Content-Length, Content-Type, Host, etc.
Parse query parameters: Extrahera city=Stockholm från URL:en
Parse body: Om POST-request, läs request body
Input: Rå text från socket ("GET /weather?city=Stockholm HTTP/1.1\r\nHost: localhost\r\n\r\n")
Output: Strukturerad data (metod, path, parametrar, headers)
*/