//Main header file for the XDK
#include <xtl.h>
#include <StdIO.h>

// Simple function to save data, feeding info into a txt file in the xbe's directory
static void Debug(char* str)
{
    FILE* fp;
    fp = fopen("D:\\DebugInfo.txt", "a+");
    fprintf(fp, "%s\n", str);
    fclose(fp);
}

static void Initialize()
{
    // Initialize main Functions
    Debug("Starting up network");

    XNetStartupParams xnsp;
    memset(&xnsp, 0, sizeof(xnsp));
    xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
    xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
    INT err = XNetStartup(&xnsp);

    DWORD TIMELIMIT = 6000;

    DWORD dwStart = GetTickCount();
    Debug("Network started, wait for 6 seconds");
    while ((GetTickCount() - dwStart) < TIMELIMIT)
    {
        // Wait it out in here, looping endlessly
        // Could we Sleep in here, or would that interrupt the tick?
    }
}

static void Connect()
{
    WSADATA WsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &WsaData);

    if (iResult != NO_ERROR)
    {
        Debug("Error at WSAStartup");
    }
    
    char aa[5000];
    SOCKET m_socket;
    m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    int whathappened = WSAGetLastError();

    sprintf(aa, "SOCKET = %ld\n", m_socket);
    Debug(aa);
    sprintf(aa, "Whathappened= %ld\n", whathappened);
    Debug(aa);
    if (m_socket == INVALID_SOCKET)
    {
        Debug("Error at socket()");
        WSACleanup();
        return;
    }
    
    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("172.217.19.164"); // google.com
    // Can alternatively use Google's DNS (8.8.8.8), yahoo.com (66.218.71.198), Wikimedia (103.102.166.224), or bbc.co.uk (212.58.253.67)
    // Response will likely be some kind of error, legible or not
    service.sin_port = htons(80);

    int results = connect(m_socket,(sockaddr*) &service,sizeof(struct sockaddr));

    if (results == SOCKET_ERROR)
    {
        Debug("Error at connect()");
    }

    send(m_socket, "GET / \r\n", strlen("GET / \r\n"), 0);

    int rr = 1;
    while (rr)
    {
        rr = recv(m_socket, aa, 500, 0);
        Debug("recv:");
        Debug(aa);
    }
}

static void CleanUp()
{
    Debug("exiting");
    // Shutdown Winsock
    WSACleanup();
    XNetCleanup();
}

// Does what it says on the tin.
// For homebrew stuff, this may not be instantly obvious that it's working
static void Reboot()
{
    LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
    XLaunchNewImage(NULL, (LAUNCH_DATA*)&LaunchData);
}

// Application entry point
void __cdecl main()
{
    // Initialize our Xbox, so we can connect to the network card
    Initialize();

    // Connect to our socket, get the html from google and save to a txt
    Connect();

    // Tidy up after ourselves
    CleanUp();

    // Reboot the Xbox when it's all done! This is how you'll know the program is finished
    Reboot();

    // When it's done, look for a txt file next to the xbe on your console.
    // It may be mostly gibberish (websites don't like being connected directly by an IP address these days)
    // but it SHOULD be a response, which is all we're testing for!
}