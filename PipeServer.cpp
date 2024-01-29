#include "pch.h"
#include "PipeServer.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>
#include <vector>
#include "feature.h"

#define BUFSIZE 512 * sizeof(TCHAR)

void GetAnswerToRequest(std::vector<uint8_t> &pchRequest, std::vector<uint8_t> &pchReply, int *pchBytes);

DWORD WINAPI PipeThread(LPVOID lpvParam)
{
    // DWORD pid = GetCurrentProcessId();
    DWORD pid = 0;

    bool fConnected = false;
    DWORD dwThreadId = 0;
    HANDLE hPipe = INVALID_HANDLE_VALUE;
    TCHAR lpszPipename[64];
    _sntprintf_s(lpszPipename, sizeof(lpszPipename), TEXT("\\\\.\\pipe\\chenstack_trainer_%ld"), pid);

    // The main loop creates an instance of the named pipe and
    // then waits for a client to connect to it. When the client
    // connects, a thread is created to handle communications
    // with that client, and this loop is free to wait for the
    // next client connect request. It is an infinite loop.

    _tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
    hPipe = CreateNamedPipe(
        lpszPipename,               // pipe name
        PIPE_ACCESS_DUPLEX,         // read/write access
        PIPE_TYPE_MESSAGE |         // message type pipe
            PIPE_READMODE_MESSAGE | // message-read mode
            PIPE_WAIT,              // blocking mode
        PIPE_UNLIMITED_INSTANCES,   // max. instances
        BUFSIZE,                    // output buffer size
        BUFSIZE,                    // input buffer size
        0,                          // client time-out
        NULL);                      // default security attribute

    if (hPipe == INVALID_HANDLE_VALUE || hPipe == NULL)
    {
        _tprintf(TEXT("CreateNamedPipe failed, LastError=%d.\n"), GetLastError());
        return -1;
    }

    // Wait for the client to connect; if it succeeds,
    // the function returns a nonzero value. If the function
    // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.

    fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (fConnected)
    {
        printf("Client connected.\n");

        DWORD cbBytesRead = 0, cbWritten = 0;
        int cbReplyBytes = 0;
        BOOL fSuccess = FALSE;

        std::vector<uint8_t> pchRequest;
        std::vector<uint8_t> pchReply;

        pchRequest.resize(BUFSIZE);
        pchReply.resize(BUFSIZE);

        // Loop until done reading
        while (true)
        {
            // Read client requests from the pipe. This simplistic code only allows messages
            // up to BUFSIZE characters in length.
            fSuccess = ReadFile(
                hPipe,                   // handle to pipe
                pchRequest.data(),       // buffer to receive data
                BUFSIZE, // size of buffer
                &cbBytesRead,            // number of bytes read
                NULL);                   // not overlapped I/O

            if (!fSuccess || cbBytesRead == 0)
            {
                if (GetLastError() == ERROR_BROKEN_PIPE)
                {
                    _tprintf(TEXT("InstanceThread: client disconnected, LastError=%d. \n"), GetLastError());
                }
                else
                {
                    _tprintf(TEXT("InstanceThread ReadFile failed, LastError=%d.\n"), GetLastError());
                }
                break;
            }
            // 处理你接收到的字符串
            // Process the incoming message.
            GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);

            if (cbReplyBytes > 0)
            {
                // Write the reply to the pipe.
                fSuccess = WriteFile(
                    hPipe,           // handle to pipe
                    pchReply.data(), // buffer to write from
                    cbReplyBytes,    // number of bytes to write
                    &cbWritten,      // number of bytes written
                    NULL);           // not overlapped I/O

                if (!fSuccess || cbReplyBytes != cbWritten)
                {
                    _tprintf(TEXT("InstanceThread WriteFile failed, LastError=%d.\n"), GetLastError());
                    break;
                }
            }
        }

        // Flush the pipe to allow the client to read the pipe's contents
        // before disconnecting. Then disconnect the pipe, and close the
        // handle to this pipe instance.

        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);

        printf("InstanceThread exitting.\n");
    }
    else
    {
        // The client could not connect, so close the pipe.
        CloseHandle(hPipe);
    }

    return 0;
}

void GetAnswerToRequest(std::vector<uint8_t> &pchRequest, std::vector<uint8_t> &pchReply, int *pchBytes)
{
    PalPipeRequest &code = *(PalPipeRequest *)pchRequest.data();
    uint8_t *data_ptr = pchRequest.data() + sizeof(PalPipeRequest);
    switch (code)
    {
    case PalPipeRequest::AddItem:
        _tprintf(TEXT("AddItem %s. \n"), reinterpret_cast<TCHAR *>(data_ptr));
        break;
    case PalPipeRequest::AddPal:
        _tprintf(TEXT("AddPal %s. \n"), reinterpret_cast<TCHAR *>(data_ptr));
        break;
    case PalPipeRequest::Teleport:
    {
        float *array = reinterpret_cast<float *>(data_ptr);
        _tprintf(TEXT("Teleport (%f, %f, %f). \n"), array[0], array[1], array[2]);
        break;
    }
    default:
        break;
    }
    *pchBytes = 0;
}