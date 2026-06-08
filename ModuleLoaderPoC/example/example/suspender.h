#pragma once
#include <windows.h>
#include <vector>

class ThreadSuspender
{
private:
  std::vector<HANDLE> suspendedThreads;

public:
  // Suspend all threads in the current process except the one calling this function
  void SuspendAllExceptCurrent();

  // Resume all previously suspended threads
  void ResumeAll();
};
