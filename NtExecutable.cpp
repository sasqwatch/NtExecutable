#include <winternl.h>

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtDelayExecution(_In_ BOOLEAN Alertable,
                 _In_ LARGE_INTEGER *Interval);

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtDisplayString(_In_ PUNICODE_STRING String);

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtTerminateProcess(_In_opt_ HANDLE ProcessHandle,
                   _In_ NTSTATUS ExitStatus);


EXTERN_C
void __cdecl
entry_point()
{
  // لايمكن طباعة هذا النص بعد تولي المشغل مسؤولية العرض، لكن يمكنك طباعته أثناء إقلاع النظام باستخدام
  // smss.exe
  UNICODE_STRING message;
  RtlInitUnicodeString(&message, L"NT <3 :D");

  // اقتباس من كتاب Windows NT/2000 Native API Reference
  //
  // يلزم تصريح SeTcbPrivilege
  // SeTcbPrivilege is required to display a string.
  //
  // يمكن عرض النصوص فقط قبل أن يتولى درايفر العرض الشاشة
  // ZwDisplayString only displays the string if the HAL still owns the display (before the
  // display driver takes ownership) or if a crash dump is in progress.
  NtDisplayString(&message);

  // كي نعرف أن الكود يعمل سننتظر لمدة عشرة ثواني
  LARGE_INTEGER internal;
  internal.QuadPart = 10 * -10000000;

  NtDelayExecution(FALSE, &internal);

  // إنهاء العملية
  NtTerminateProcess(nullptr, 0);
}
