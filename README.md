## Windows Native Subsystem Executable

This repo holds a sample Native Subsystem executable (`bin/NtExecutable.exe`).
It cannot be lunched directly, so I wrote another tool (`bin/NtExecute.exe`)
to lunch the executable under Win32 Subsystem.

`NtExecutable.exe` uses `NtDisplayString()` which can't be used once the display driver owns the display
after the boot has been completed. `NtExecutable.exe` still suspends its execution for 10 seconds so you
still know it is working.

You can see the output of `NtDisplayString()` by using `smss.exe` 
([Session Manager Subsystem](https://en.wikipedia.org/wiki/Session_Manager_Subsystem)) which executes 
any program (like `autocheck`) listed under the Registry key:

    HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\BootExecute

or:

    HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\SETUPEXECUTE

Just copy the `NtExecutable.exe` to `C:\Windows` and append `NtExecutable` to either one of these two.

![Boot](https://raw.githubusercontent.com/Barakat/NtExecutable/master/boot.png)

Random Notes:

* MinGW was used for building the binaries (only x64)
* Code was tested on Microsoft Windows 10.0.10586
* Trust nobody, run the code under some VM


    0eb6ebd69f800f80217aeeb7d48935d9 *NtExecutable.exe
    21f261385cb7ad447f332d19f14b3763 *NtExecute.exe
