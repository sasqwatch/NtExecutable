#include <windows.h>
#include <winternl.h>
#include <cassert>
#include <cwchar>

//
//
// تراكيب بيانات غير موثقة
//
//
typedef struct _SECTION_IMAGE_INFORMATION
{
  PVOID TransferAddress;
  ULONG ZeroBits;
  SIZE_T MaximumStackSize;
  SIZE_T CommittedStackSize;
  ULONG SubSystemType;
  union
  {
    struct
    {
      USHORT SubSystemMinorVersion;
      USHORT SubSystemMajorVersion;
    } SubSystemSubVersion;
    ULONG SubSystemVersion;
  };
  union
  {
    struct
    {
      USHORT MajorOperatingSystemVersion;
      USHORT MinorOperatingSystemVersion;
    } OperatingSystemSubVersion;
    ULONG OperatingSystemVersion;
  };
  USHORT ImageCharacteristics;
  USHORT DllCharacteristics;
  USHORT Machine;
  BOOLEAN ImageContainsCode;
  union
  {
    UCHAR ImageFlags;
    struct
    {
      UCHAR ComPlusNativeReady
        : 1;
      UCHAR ComPlusILOnly
        : 1;
      UCHAR ImageDynamicallyRelocated
        : 1;
      UCHAR ImageMappedFlat
        : 1;
      UCHAR BaseBelow4gb
        : 1;
      UCHAR ComPlusPrefer32bit
        : 1;
      UCHAR Reserved
        : 2;
    } ImageFlagsBits;
  };
  ULONG LoaderFlags;
  ULONG ImageFileSize;
  ULONG CheckSum;
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

typedef struct _RTL_USER_PROCESS_INFORMATION
{
  ULONG Size;
  HANDLE ProcessHandle;
  HANDLE ThreadHandle;
  CLIENT_ID ClientId;
  SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;

//
//
// دوال غير موثقة
//
//
EXTERN_C
NTSYSAPI
NTSYSAPI
ULONG
RtlNtStatusToDosErrorNoTeb(NTSTATUS Status);

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
RtlCreateProcessParameters(_Out_ PRTL_USER_PROCESS_PARAMETERS *ProcessParameters,
                           _In_ PUNICODE_STRING ImagePathName,
                           _In_opt_ PUNICODE_STRING DllPath,
                           _In_opt_ PUNICODE_STRING CurrentDirectory,
                           _In_opt_ PUNICODE_STRING CommandLine,
                           _In_opt_ PWSTR Environment,
                           _In_opt_ PUNICODE_STRING WindowTitle,
                           _In_opt_ PUNICODE_STRING DesktopInfo,
                           _In_opt_ PUNICODE_STRING ShellInfo,
                           _In_opt_ PUNICODE_STRING RuntimeInfo);

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
RtlCreateUserProcess(_In_ PUNICODE_STRING ImageFileName,
                     _In_ ULONG Attributes,
                     _In_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
                     _In_opt_ PSECURITY_DESCRIPTOR ProcessSecutityDescriptor,
                     _In_opt_ PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
                     _In_opt_ HANDLE ParentProcess,
                     _In_ BOOLEAN CurrentDirectory,
                     _In_opt_ HANDLE DebugPort,
                     _In_opt_ HANDLE ExceptionPort,
                     _Out_ PRTL_USER_PROCESS_INFORMATION ProcessInfo);

// لطباعة رسائل الأخطاء
void
PrintNtError(NTSTATUS status)
{
  auto last_error = RtlNtStatusToDosErrorNoTeb(status);
  LPWSTR message;

  if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     nullptr,
                     last_error,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     reinterpret_cast<LPWSTR>(&message),
                     0,
                     nullptr) != 0)
  {
    std::fwprintf(stderr, L"%ls: %ls", NT_SUCCESS(status) ? L"INFO" : L"ERROR", message);
    LocalFree(message);
  }
}

int
main()
{
  static const auto executable_path = LR"(NtExecutable.exe)";

  NTSTATUS status;

  // نحول المسار من المسار الذي يفهمه
  // Win32/DOS (C:\foo.exe)
  // إلى المسار الذي يفهمه ويندوز داخلياً
  // NT path (\??\C:\foo.exe)
  UNICODE_STRING image_nt_path_name;
  RtlDosPathNameToNtPathName_U(executable_path, &image_nt_path_name, nullptr, nullptr);

  // نجهز معاملات العملية
  PRTL_USER_PROCESS_PARAMETERS rtl_user_process_parameters;
  status = RtlCreateProcessParameters(&rtl_user_process_parameters,
                                      &image_nt_path_name,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr);
  if (!NT_SUCCESS(status))
  {
    PrintNtError(status);
    return -1;
  }

  // ننشئ العملية
  RTL_USER_PROCESS_INFORMATION process_info {};
  process_info.Size = sizeof(process_info);

  status = RtlCreateUserProcess(&image_nt_path_name,
                                OBJ_CASE_INSENSITIVE,
                                rtl_user_process_parameters,
                                nullptr,
                                nullptr,
                                GetCurrentProcess(),
                                TRUE,
                                nullptr,
                                nullptr,
                                &process_info);
  if (!NT_SUCCESS(status))
  {
    PrintNtError(status);
    return -1;
  }

  // بعد إنشاء العملية سيتم إنشاء خيط معالجة رئيسي لكنه متوقف، نستأنف تنفيذه هنا
  ResumeThread(process_info.ThreadHandle);

  // ننتظر إلى أن تنتهي العملية، لو كان الكود سليم فسننتظر لعشرة ثواني
  WaitForSingleObject(process_info.ProcessHandle, INFINITE);

  return 0;
}
