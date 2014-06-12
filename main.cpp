/*  INFO
*   name: IVERA_interface_window
*   version beta: 15
*   S. Kamps
*	Toegevoegd Button Rood, GEKN, INTERP_REC
*/
using namespace std;
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <winsock.h>
#include "resource.h"


//Prototypes
BOOL CALLBACK DlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam); // Main window
BOOL CALLBACK ConnectDlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam); // client window
void IVERACOMMAND(const char* Data); // Pass IVERA commands
void GetTextandAddLine(char Line[], HWND hParent, int IDC); // Display text
void INTERP_REC(char buffer[]); // Interpreted received IVERA message from VRI
int TryConnect(long hostname, int PortNo); // Client | Edit portno disabled, Now, always IVERA port 5000

char Title[] = "IVERA_interface";

HINSTANCE hInst = NULL;
HWND hwnd, hStatus;

SOCKET sConnect;
SOCKADDR_IN local;
int locallen = sizeof(local);

// Start Code

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
	  hInst = hInstance;

	  return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN),
   						NULL, DlgProc); // MAIN dialog box

}


// Main window
BOOL CALLBACK DlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam)
{
switch(Message)
    {

    case WM_INITDIALOG:
    {
        //Our dialog box is being created
        hwnd = hdwnd;
        hStatus = GetDlgItem(hdwnd, ID_STATUS_MAIN);
    }
    return TRUE;

    case WINSOCK_EVENT: //Winsock related message (1045)
        switch (lParam)
        {
                case FD_CLOSE: //Lost connection
                    MessageBeep(MB_ICONERROR);
                    if (sConnect)
                    {
                    closesocket(sConnect);
                    WSACleanup(); //Clean up
                    SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Connection to VRI Lost.");
			     	}
                break;

                case FD_READ: //Incoming data to receive
                    char buffer[128];
                    memset(buffer, 0, sizeof(buffer)); //Clear the buffer
                    recv (sConnect, buffer, sizeof(buffer)-1, 0); //Get the text
					INTERP_REC(buffer); // Interpreted received IVERA message from VRI before displaying it (added v14)
                    //GetTextandAddLine(buffer, hwnd, ID_EDIT_DATA); //Display it > call from INTERP_REC (added v14)
                break;
        }
    break;

    case WM_COMMAND: // Main buttons interaction
         const char* Data;
        switch(LOWORD(wParam))
        {
                case ID_BTN_CONNECT: // CONNECT dialog box
                  return DialogBox(hInst, MAKEINTRESOURCE(DLG_CONNECT),
                   						NULL, ConnectDlgProc);
                break;
                case IDCANCEL: // X to close window
                    //Clean up
                    if (sConnect)
					{
					closesocket(sConnect);
                    WSACleanup();
					}
                    EndDialog(hdwnd, IDOK);
                break;
                case ID_BTN_CLOSE: // Close button (v16)
                	if (sConnect)
					{
					int a = MessageBox(hdwnd, "Are you sure you want to end the current connection?",
                                        "End Connection", MB_ICONQUESTION | MB_YESNO);
                        if (a == IDYES)
                        {
                            SendDlgItemMessage(hdwnd, ID_EDIT_DATA, WM_SETTEXT, 0, (LPARAM)"");
                            SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"");
                            closesocket(sConnect); //Shut down socket
                            WSACleanup(); //Clean up Winsock
                        }
					}
                	DestroyWindow(hdwnd);
                break;
                case ID_BTN_CLEAR: //Clear edit and disconnect
                {
                    if (sConnect) //If there's a connection
                    {
                        int a = MessageBox(hdwnd, "Are you sure you want to end the current connection?",
                                        "End Connection", MB_ICONQUESTION | MB_YESNO);
                        if (a == IDYES)
                        {
                            SendDlgItemMessage(hdwnd, ID_EDIT_DATA, WM_SETTEXT, 0, (LPARAM)"");
                            SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)"");
                            closesocket(sConnect); //Shut down socket
                            WSACleanup(); //Clean up Winsock
                        }
                    }
                }
                break;
                case ID_BTN_ABOUT:
                   	return MessageBox(hdwnd, "IVERA interface v16\n""Written by Sander Kamps\n""2013/2014",
                                        "About", MB_ICONQUESTION | MB_OK);
                break;
                case ID_BTN_SEND: //Send data
                {
                    int Textlength = GetWindowTextLength(GetDlgItem(hdwnd, ID_EDIT_SEND)); //Get length of text in send box

          	        if (Textlength) //If there's text in the reply box...
			        {
			            if (sConnect) // Sock ok?
			            {
			                char* Data;
			                Data = (char*)GlobalAlloc(GPTR, Textlength+1);
			                GetDlgItemText(hdwnd, ID_EDIT_SEND, Data, Textlength+1); // Get text with Textlenght from send box and put it in Data
			                char szTemp[Textlength+1]; // Buffer to hold Text from send box
			                wsprintf(szTemp, "%s\r\n", Data); // Put Data in Buffer
			                send(sConnect, szTemp, strlen(szTemp), 0); //Send the string
			                SetDlgItemText(hdwnd, ID_EDIT_SEND, ""); //Reset the textbox
			                GlobalFree((HANDLE)Data); //Free memory
			            }
			            else
			            {
			            //We aren't connected!!
			            MessageBox(hwnd, "No established connection detected.",
			              Title, MB_ICONERROR | MB_OK);
			            }
			        }
                }
                break;

				case ID_BTN_AUTBED:
                      IVERACOMMAND(AUTBED);
				break;
				case ID_BTN_TKL:
                      IVERACOMMAND(TKL);
				break;
				case ID_BTN_VERST1:
                      IVERACOMMAND(VERST1);
			    break;
				case ID_BTN_VERST2:
						IVERACOMMAND(VERST2);
			    break;
			    case ID_BTN_VERST3:
						IVERACOMMAND(VERST3);
			    break;
			    case ID_BTN_VERST4:
						IVERACOMMAND(VERST4);
			    break;
				case ID_BTN_VERST5:
						IVERACOMMAND(VERST5);
			    break;
			    case ID_BTN_VERST6:
						IVERACOMMAND(VERST6);
			    break;
			    case ID_BTN_VERST7:
						IVERACOMMAND(VERST7);
			    break;
			    case ID_BTN_VERST8:
						IVERACOMMAND(VERST8);
			    break;
			    case ID_BTN_VERST9:
						IVERACOMMAND(VERST9);
			    break;
			    case ID_BTN_VERST10:
						IVERACOMMAND(VERST10);
			    break;
			    case ID_BTN_VERST15:
						IVERACOMMAND(VERST15);
			    break;
			    case ID_BTN_Prg01:
						IVERACOMMAND(Prg01);
			    break;
			    case ID_BTN_Prg02:
						IVERACOMMAND(Prg02);
			    break;
			    case ID_BTN_Prg03:
						IVERACOMMAND(Prg03);
			    break;
			    case ID_BTN_Prg04:
						IVERACOMMAND(Prg04);
				break;
				case ID_BTN_ROOD:
						IVERACOMMAND(ROOD);
				break;
				case ID_BTN_GEKN:
						IVERACOMMAND(GEKN);
			    break;
			break;
        } //End switch
        default:
            return FALSE;
    break;
    } //End Message switch
    return TRUE;

}
// Cliend Dialog
BOOL CALLBACK ConnectDlgProc(HWND hdwnd, UINT Message, WPARAM wParam, LPARAM
  lParam)
{
switch(Message)
    {
    case WM_INITDIALOG:
    {
        //Our dialog box is being created
    }
    return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
                case ID_BTN_GO:
                {
                    int len = GetWindowTextLength(GetDlgItem(hdwnd, ID_EDIT_HOST)); // Get host IP
                    int portno = 5000;//GetDlgItemInt(hdwnd, ID_EDIT_PORT, 0, 0); // Client | Edit portno disabled, Now, always IVERA port 5000

                    if (len)
                    {
                            char* Data;
                            Data = (char*)GlobalAlloc(GPTR, len + 1); //Allocate memory
                            GetDlgItemText(hdwnd, ID_EDIT_HOST, Data, len + 1); //Get text into buffer
                            if (!gethostbyname(Data))
                            {
                            //Couldn't get hostname; assume it's an IP Address
                                long hostname = inet_addr(Data);
                                if(!TryConnect(hostname, portno))
                                {
                                    MessageBox(hdwnd, "Could not connect to remote host.", Title, MB_ICONERROR | MB_OK);
                                    if (sConnect)
                                    {
									closesocket(sConnect); //Shut down socket
									WSACleanup(); //Clean up Winsock - Added Beta v14
									}
                                }
                            }

                            GlobalFree((HANDLE)Data); //Free memory
                            EndDialog(hdwnd, IDOK);
                    }
                }
                break;

                case IDCANCEL: // X to close window
                    EndDialog(hdwnd, IDOK);
                break;
        } //End switch
        default:
            return FALSE;
    break;
    } //End Message switch
    return TRUE;
}


// Connecting Client procedure
int TryConnect(long hostname, int PortNo) // Client | Edit portno disabled, Now, always IVERA port 5000
{
    WSADATA wsadata; //Winsock startup info
    SOCKADDR_IN local; //Information about host

    int error = WSAStartup (0x0202, &wsadata);   // Fill in WSA info

    if (error)
    { // there was an error
      return 0;
    }
    if (wsadata.wVersion != 0x0202)
    { // wrong WinSock version!
      WSACleanup (); // unload ws2_32.dll
      return 0;
    }

    sConnect = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create socket
    if (sConnect == INVALID_SOCKET)
    {
        return 0;
    }
    local.sin_family = AF_INET;           // address family Internet
    local.sin_port = htons (PortNo);       // set servers port number
    local.sin_addr.s_addr = hostname;  // set servers IP
    //Try connecting...
    if (connect(sConnect, (SOCKADDR *)&local, sizeof(local)) == SOCKET_ERROR) //Try binding
    { // error
          return 0;
    }
    WSAAsyncSelect (sConnect, hwnd, WINSOCK_EVENT, FD_READ | FD_CLOSE); //Switch to Non-Blocking mode
    char szTemp[12];
 	wsprintf(szTemp, "Connected to %s",inet_ntoa(local.sin_addr));
    SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)szTemp);
    // Initial Dialog to VRI to LOGIN and get VRIID
                    char Temp1[strlen(LOGIN)]; // Buffer to hold Text from send box
			              wsprintf(Temp1, "%s\r\n", LOGIN); // Put Data in Buffer
			              send(sConnect, Temp1, strlen(Temp1), 0); //Send the string
                    char Temp2[strlen(VRIID)]; // Buffer to hold Text from send box
			              wsprintf(Temp2, "%s\r\n", VRIID); // Put Data in Buffer
			              send(sConnect, Temp2, strlen(Temp2), 0); //Send the string
                    char Temp3[strlen(VRISTAT)]; // Buffer to hold Text from send box
			              wsprintf(Temp3, "%s\r\n", VRISTAT); // Put Data in Buffer
			              send(sConnect, Temp3, strlen(Temp3), 0); //Send the string
                    char Temp4[strlen(VRIPROG)]; // Buffer to hold Text from send box
			              wsprintf(Temp4, "%s\r\n", VRIPROG); // Put Data in Buffer
			              send(sConnect, Temp4, strlen(Temp4), 0); //Send the string
    return 1;
}

void IVERACOMMAND(const char* Data)
{

			             char szTemp[strlen(Data)]; // Buffer to hold Text from send box
			             wsprintf(szTemp, "%s\r\n", Data); // Put Data in Buffer
			             send(sConnect, szTemp, strlen(szTemp), 0); //Send the string
			             GlobalFree((HANDLE)Data); //Free memory
                   return;
}

// Display text
void GetTextandAddLine(char Line[], HWND hParent, int IDC)
{
    HWND hEdit = GetDlgItem(hParent, IDC);
	int nTxtLen = GetWindowTextLength(hEdit); 				// Get length of existing text
	SendMessage(hEdit, EM_SETSEL, nTxtLen, nTxtLen);		// move caret to end
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)Line);	    // append text
	SendMessage(hEdit, EM_SCROLLCARET, 0, 0);				// scroll to caret
}

void INTERP_REC(char buffer[]) // Interpreted received IVERA message from VRI
{
char *pOUTPUT; // Pass to GetTextandAddLine() which needs an char array
string COMPOSED_string; // string composed out of substrings and additions

std::string string_source(buffer);
std::string string_LOGIN("LOGIN/#0=4444");
std::string string_VRIID("VRIID");
std::string string_VRISTAT("VRISTAT");
std::string string_VRIPROG("VRIPROG");

std::size_t found=string_source.find(string_LOGIN); // Find LOGIN in IVERA data (string_source) to extract ID info
  if (found!=std::string::npos)
  	{
	  GetTextandAddLine("", hwnd, ID_EDIT_DATA); //Display COMPOSED_string
	  memset(buffer, 0, sizeof(buffer)); //Clear the buffer
 	}

found=string_source.find(string_VRIID); // Find VRIID in IVERA data (string_source) to extract ID info
  if (found!=std::string::npos)
  	{
  		//memset(buffer, 0, sizeof(buffer)); //Clear the buffer
		string VT_NUMMER = string_source.substr(9,6); // Extract VT_NUMMER
		string KR_NUMMER = string_source.substr(18,4); // Extract KR_NUMMER
		COMPOSED_string = "KR"; COMPOSED_string+=KR_NUMMER; COMPOSED_string+="    VT"; COMPOSED_string+=VT_NUMMER; COMPOSED_string+="\n";// COMPOSED_string is KRxxx VTxxxxxx
		pOUTPUT = strdup(COMPOSED_string.c_str()); // Copy string COMPOSED_string to char array *pOUTPUT in order to pass to GetTextandAddLine()
		GetTextandAddLine(pOUTPUT, hwnd, ID_EDIT_DATA); //Display pOUTPUT
		SendMessage(hStatus, WM_SETTEXT, 0, (LPARAM)pOUTPUT);// Display VT_nummer and KR_nummer in Windowbar
	}


found=string_source.find(string_VRISTAT); // Find VRISTAT in IVERA data (string_source) to extract STATUS info
  if (found!=std::string::npos)
  	{
	  string VRISTAT = string_source.substr(8,2); // Extract VRISTAT = vri status first entry
		if 		(VRISTAT=="00") COMPOSED_string ="status is: Automatisch bedrijf\n";
		else if (VRISTAT=="01") COMPOSED_string ="status is: Buiten bedrijf\n";
		else if (VRISTAT=="02") COMPOSED_string ="status is: Gedoofd\n";
		else if (VRISTAT=="03") COMPOSED_string ="status is: Knipperen (extern)\n";
		else if (VRISTAT=="04") COMPOSED_string ="status is: Knipperen\n";
		else if (VRISTAT=="05") COMPOSED_string ="status is: Knipperen tijdens inschakelen\n";
		else if (VRISTAT=="06") COMPOSED_string ="status is: Vastgeel\n";
		else if (VRISTAT=="07") COMPOSED_string ="status is: Alles rood tijdens inschakelen\n";
		else if (VRISTAT=="08") COMPOSED_string ="status is: Alles rood\n";
		else if (VRISTAT=="09") COMPOSED_string ="status is: Alles rood tijdens uitschakelen\n";
		else if (VRISTAT=="10") COMPOSED_string ="status is: Regelen\n";
	  pOUTPUT = strdup(COMPOSED_string.c_str()); // Copy string COMPOSED_string to char array *pOUTPUT in order to pass to GetTextandAddLine()
	  GetTextandAddLine(pOUTPUT, hwnd, ID_EDIT_DATA); //Display pOUTPUT
  	}
found=string_source.find(string_VRIPROG); // Find VRIPROG in IVERA data (string_source) to extract PROGRAM info
  if (found!=std::string::npos)
  	{
	  string VRIPROG = string_source.substr(8,1); // Extract VRIPROG = current running program
  	  COMPOSED_string = "programma is: "; COMPOSED_string+=VRIPROG; COMPOSED_string+="\n";
  	  pOUTPUT = strdup(COMPOSED_string.c_str()); // Copy string COMPOSED_string to char array *pOUTPUT in order to pass to GetTextandAddLine()
  	  GetTextandAddLine(pOUTPUT, hwnd, ID_EDIT_DATA); //Display pOUTPUT
  	}
// GetTextandAddLine(buffer, hwnd, ID_EDIT_DATA); //Display it > call from INTERP_REC (added v14)

  	return;
}
