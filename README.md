\## Building VIOFetch



VIOFetch was tested under MS OS/2 1.3 Standard Edition in VirtualBox and built using Microsoft C 6.00.



Three OS/2 command scripts are included:



\- `CC.CMD` — Compile a C source file.

\- `LL.CMD` — Link the object file into an executable, using a `.DEF` file if present.

\- `BUILD.CMD` — Run compilation followed by linking.



To build VIOFetch, run:



```cmd

BUILD VIOFETCH

```



The build scripts expect the Microsoft C 6.00 compiler and linker to be available. `LL.CMD` references libraries under `C:\\C600\\LIB`, so adjust that path if your installation differs.

