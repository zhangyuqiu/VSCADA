#include "glv_thread.h"

void GLV_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
}
