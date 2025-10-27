#include "../libs/TCPClient.h"
	
	int main()
	{

		char *URL = "http://www.google.com";

		
		TCPClient client;
		uint8_t buffer[1024];
		const char* message = "Hello from client!\r\n";
		int length = (int)strlen(message);
		TCPClient_Initiate(&client);
		if (TCPClient_Connect(&client, "localhost", "8080") != 0)
		{
			printf("Could not connect to server\r\n");
			return -1;
		}

        int totalBytesRead = 0;
		// while(1)
		{
            int bytesWritten = TCPClient_Write(&client, (const uint8_t*)message, strlen(message));
            printf("Bytes written: %d\r\n", bytesWritten);

            while(totalBytesRead < length)
            {
                int bytesRead = TCPClient_Read(&client, buffer, sizeof(buffer) - 1);
                if (bytesRead > 0)
                {
                    totalBytesRead += bytesRead;
                    buffer[bytesRead] = '\0'; // Null-terminate string
                    printf("%s", buffer);
                }
            }
		}
		
		TCPClient_Dispose(&client);

		return 0;
	}