#include "suspender.h"
#include <tlhelp32.h>

void ThreadSuspender::SuspendAllExceptCurrent()
{
  DWORD currentProcessId = GetCurrentProcessId();
  DWORD currentThreadId = GetCurrentThreadId();

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
    return;

  THREADENTRY32 te;
  te.dwSize = sizeof(te);

  if (Thread32First(snapshot, &te))
  {
    do
    {
      if (te.th32OwnerProcessID == currentProcessId && te.th32ThreadID != currentThreadId)
      {
        HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
        if (hThread)
        {
          if (SuspendThread(hThread) != (DWORD)-1)
          {
            suspendedThreads.push_back(hThread); // save handle to resume later
          }
          else
          {
            CloseHandle(hThread); // cleanup if suspend fails
          }
        }
      }
    } while (Thread32Next(snapshot, &te));
  }

  CloseHandle(snapshot);
}

void ThreadSuspender::ResumeAll()
{
  for (HANDLE hThread : suspendedThreads)
  {
    ResumeThread(hThread);
    CloseHandle(hThread); // release handle
  }
  suspendedThreads.clear();
}
