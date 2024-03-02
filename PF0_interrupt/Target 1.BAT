SET PATH=D:\mps soft\ARM\ARMCC\bin\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;D:\MatLab R2021a\runtime\win64;D:\MatLab R2021a\bin;D:\MatLab R2021a\polyspace\bin;C:\Program Files\dotnet\;C:\Program Files\bladeRF\x64;D:\Putty\;C:\Users\Zawah\AppData\Local\Microsoft\WindowsApps
SET CPU_TYPE=TM4C1233H6PM
SET CPU_VENDOR=Texas Instruments
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x00F42400
"D:\mps soft\ARM\ARMCC\bin\ArmAsm" --Via ".\startup_tm4c123._ia"
"D:\mps soft\ARM\ARMCC\bin\ArmCC" --Via ".\pf0_led.__i"
"D:\mps soft\ARM\ARMCC\bin\ArmLink" --Via ".\PF0_LED.lnp"
D:\mps soft\ARM\ARMCC\bin\fromelf.exe ".\PF0_LED.axf" --i32combined --output ".\PF0_LED.hex"
