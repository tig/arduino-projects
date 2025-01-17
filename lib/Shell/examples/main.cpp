
/*
This example demonstrates how to create a simple telnet server.

This example is placed into the public domain.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Shell.h>
#include <LoginShell.h>

byte macAddress[6] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

int ledPin = 13;

EthernetServer shellServer(23);
EthernetClient shellClient;
bool haveClient = false;

Shell shell;

static int noSecurityPasswordCheckFunc(const char *username, const char *password)
{
    return 0;
}


void cmdLed(Shell &shell, int argc, const ShellArguments &argv)
{
    if (argc > 1 && !strcmp(argv[1], "on")){
        digitalWrite(ledPin, HIGH);
    }
    else if (argc > 1 && !strcmp(argv[1], "off")){
        digitalWrite(ledPin, LOW);
    }
    else if  (argc > 1 && !strcmp(argv[1], "?")){
        shell.print("led set to ");
        shell.println(digitalRead(ledPin), DEC);
    }

}

ShellCommand(led, "Turns the status LED on or off", cmdLed);

void setup()
{
    // Configure I/O.
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // Start the serial port for status messages.
    Serial.begin(115200);
    Serial.println();
    Serial.print("Acquiring IP address ... ");

    // Start Ethernet running and get an IP address via DHCP.
    if (Ethernet.begin(macAddress))
        Serial.println(Ethernet.localIP());
    else
        Serial.println("failed");

    // Listen on port 23 for incoming telnet connections.
    shellServer.begin();
    //shell.setPasswordCheckFunction(&noSecurityPasswordCheckFunc);
    //shell.setMachineName("Arduino");
    shell.setPrompt(">");
}

void loop()
{
    // Maintain the DHCP lease over time.
    Ethernet.maintain();

    // Handle new/disconnecting clients.
    if (!haveClient) {
        // Check for new client connections.
        shellClient = shellServer.available();
        if (shellClient) {
            haveClient = true;
            shell.begin(shellClient, 5);
        }
    } else if (!shellClient.connected()) {
        // The current client has been disconnected.  Shut down the shell.
        shell.end();
        shellClient.stop();
        shellClient = EthernetClient();
        haveClient = false;
    }

    // Perform periodic shell processing on the active client.
    shell.loop();
}
