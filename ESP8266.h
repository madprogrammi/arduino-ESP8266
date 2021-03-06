#ifndef ESP8266_h
#define ESP8266_h

#include <Arduino.h>
#include <IPAddress.h>

#define ESP8266_SINGLE_CLIENT 5

enum ESP8266WifiMode {
    ESP8266_WIFI_STATION = 1,
    ESP8266_WIFI_ACCESSPOINT,
    ESP8266_WIFI_BOTH
};

enum ESP8266Encryption {
    ESP8266_ENCRYPTION_OPEN = 0,
    ESP8266_ENCRYPTION_WEP,
    ESP8266_ENCRYPTION_WPA_PSK,
    ESP8266_ENCRYPTION_WPA2_PSK,
    ESP8266_ENCRYPTION_WPA_WPA2_PSK
};

enum ESP8266ConnectionStatus {
    ESP8266_CONNECTION_GOT_IP = 2,
    ESP8266_CONNECTION_CONNECTED,
    ESP8266_CONNECTION_DISCONNECTED
};

enum ESP8266Protocol {
    ESP8266_PROTOCOL_TCP = 0,
    ESP8266_PROTOCOL_UDP
};

enum ESP8266Role {
    ESP8266_ROLE_CLIENT = 0,
    ESP8266_ROLE_SERVER
};

enum ESP8266CommandStatus {
    ESP8266_COMMAND_INVALID = -2,
    ESP8266_COMMAND_TIMEOUT,
    ESP8266_COMMAND_OK,
    ESP8266_COMMAND_NO_CHANGE,
    ESP8266_COMMAND_ERROR,
    ESP8266_COMMAND_NO_LINK,
    ESP8266_COMMAND_TOO_LONG
};

enum ESP8266ServerMode {
    ESP8266_SERVER_DELETE = 0,
    ESP8266_SERVER_CREATE
};

struct ESP8266Connection {
    uint8_t id;
    ESP8266Protocol protocol;
    IPAddress ip;
    uint16_t port;
    ESP8266Role role;
};

struct ESP8266Station {
    IPAddress ip;
    byte mac[6];
};

//TODO: ESP8266AccessPoint struct

class ESP8266
{
public:
    ESP8266()
    {
        _timeout=1000;
    }

    // Set the underlying serial and prepare the module
    bool begin(Stream &serial);

    // Set timeout
    void setTimeout(unsigned long timeout);

    /****************************************/
    /******       Basic commands       ******/
    /****************************************/
    // Test AT startup
    ESP8266CommandStatus test();

    // Restart module. Returns false if an error occurred
    bool restart();

    // Get version info
    ESP8266CommandStatus getVersion(char* buffer, int length);

    // Enter deep-sleep mode
    ESP8266CommandStatus deepSleep(unsigned long time);

    // Set baudrate to the given value
    void setBaudrate(unsigned long baudrate);

    /****************************************/
    /******        WiFi commands       ******/
    /****************************************/
    // Set WiFi mode
    ESP8266CommandStatus setWifiMode(ESP8266WifiMode mode);

    // Get the WiFi mode
    ESP8266CommandStatus getWifiMode(ESP8266WifiMode* mode);

    // Join the access point
    ESP8266CommandStatus joinAP(char* ssid, char* password);

    // Get the current access point
    ESP8266CommandStatus getAP(char* ssid);

    // List available APs
    //TODO
    //ESP8266CommandStatus listAP();

    // List available APs matching criteria
    //TODO
    //ESP8266CommandStatus listAP(char* ssid, byte mac[6], unsigned int channel);

    // Quit the current access point
    ESP8266CommandStatus quitAP();

    // Set the configuration of the access point
    ESP8266CommandStatus setAPConfiguration(char* ssid, char* password, uint8_t channel = 6, ESP8266Encryption encryption = ESP8266_ENCRYPTION_WPA_WPA2_PSK);

    // Get access point's configuration
    ESP8266CommandStatus getAPConfiguration(char* ssid, char* password, uint8_t& channel, ESP8266Encryption& encryption);

    // Get IP of stations connected to the access point
    ESP8266CommandStatus getConnectedStations(ESP8266Station* stations, unsigned int& count, const unsigned int max);

    // Set DHCP
    ESP8266CommandStatus setDHCP(ESP8266WifiMode mode, bool enable);

    // Set the mac address
    ESP8266CommandStatus setMAC(ESP8266WifiMode mode, byte mac[6]);

    // Get the mac address
    ESP8266CommandStatus getMAC(ESP8266WifiMode mode, byte mac[6]);

    // Set the ip address
    ESP8266CommandStatus setIP(ESP8266WifiMode mode, IPAddress& ip);

    // Get the ip address
    ESP8266CommandStatus getIP(ESP8266WifiMode mode, IPAddress& ip);

    /****************************************/
    /******       TCP/IP commands      ******/
    /****************************************/
    // Get connection status
    ESP8266CommandStatus getConnectionStatus(ESP8266ConnectionStatus& status, ESP8266Connection* connection, unsigned int& count);

    // Establish connection
    ESP8266CommandStatus connect(ESP8266Protocol protocol, IPAddress ip, unsigned int port);
    ESP8266CommandStatus connect(unsigned int id, ESP8266Protocol protocol, IPAddress ip, unsigned int port);

    // Send data
    ESP8266CommandStatus send(char* data);
    ESP8266CommandStatus send(char* data, unsigned int length);
    ESP8266CommandStatus send(unsigned int id, char* data);
    ESP8266CommandStatus send(unsigned int id, char* data, unsigned int length);

    // Close connection
    ESP8266CommandStatus close();
    ESP8266CommandStatus close(unsigned int id);

    // Set multiple connections
    ESP8266CommandStatus setMultipleConnections(bool enable);

    // Get multiple connections
    ESP8266CommandStatus getMultipleConnections(bool& enable);

    // Configure server
    ESP8266CommandStatus configureServer(ESP8266ServerMode mode, unsigned int port);

    // Set server timeout
    ESP8266CommandStatus setServerTimeout(unsigned int timeout);

    // Set server timeout
    ESP8266CommandStatus getServerTimeout(unsigned int& timeout);

    // Available
    int available();

    // Connection id
    unsigned int getId();

    // Read
    int read();
    int read(uint8_t* buf, size_t size);

protected:
    Stream* _serial;

    unsigned long _timeout;

    int _available;

    unsigned int _id;

    // Clear the incomming data
    void clear();

    // Initialize the module and wait for it to be ready
    bool initialize();

    // Set the echo
    ESP8266CommandStatus setEcho(bool enable);

    // Set transmission mode
    ESP8266CommandStatus setUnvarnishedMode(bool enable);

    // Read the underlying serial, waiting for timeout milliseconds. Returns the read char or -1 if timeout
    int timedRead(unsigned int timeout);

    // Peek the underlying serial, waiting for timeout milliseconds. Returns the read char or -1 if timeout
    int timedPeek(unsigned int timeout);

    // Parse an int from serial
    int parseInt(unsigned int timeout);

    // Parse an hex byte (2 char) from serial
    byte parseHex(unsigned int timeout);

    // Parse a protocol from serial
    ESP8266Protocol parseProtocol(unsigned int timeout);

    // Parse an IP address from serial
    IPAddress parseIPAddress(unsigned int timeout);

    // Parse a MAC address from serial
    void parseMACAddress(byte mac[6], unsigned int timeout);

    // Read status
    ESP8266CommandStatus readStatus(unsigned int timeout);

    // Read bytes until target is matched or timeout occurs. Returns true if matched, false otherwise
    bool find(const __FlashStringHelper* target);
    bool find(const __FlashStringHelper* target, unsigned int timeout);

    // Read bytes into buffer until length is reached or target is matched or timeout occurs, whichever first. Returns the number of bytes placed in the buffer
    size_t readUntil(char* buffer, size_t length, const __FlashStringHelper* target, unsigned int timeout);

    // Read until a string is found. If strict is true, stop on first non-matching character
    int findStrings(const char** strings, unsigned int count, bool strict, unsigned int timeout);
};

#endif
