# Background

Microsoft Windows based programs often use shared libraries (called dynamic link libraries or DLL files) as part of a runtime environment.  There are many versions of these runtimes.
Sometimes a program can't find the version it needs.  Windows will give an error about a "procedure entry point" that "could not be located in the dynamic link library".

# Dependencies

If BiblioteQ complains about a missing DLL file, then it can probably be resolved by installing the appropriate runtime environment.
This is usually done by downloading and installing a file from Microsoft.  In some cases the DLL file mentioned is not part of a runtime, but rather is part of the OS itself,
as is often the case with such errors on Windows XP.  See [Unsupported](#unsupported) below.
  
Here are some BiblioteQ DLL dependencies that are sometimes missing, though there may be others.

- api-ms-win-crt-runtime-l1-1-0.dll
- vcruntime140.dll

# Downloads

First try this: [Visual C++ Redistributable for Visual Studio 2015](https://www.microsoft.com/en-us/download/details.aspx?id=48145) and see if that fixes it for you.

If the above doesn't seem to work, try this instead: [Update for Universal C Runtime in Windows](https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows)
and hopefully that resolves the issue.

# Unsupported

Windows XP is not supported.  Microsoft stopped supporting Windows XP several years ago.  Qt - the application framework used to build BiblioteQ - stopped supporting Windows XP.
The author of BiblioteQ has likewise abandonned Windows XP development.

If BiblioteQ works on Windows XP, that's great, but it may break at any time in the future, and likely will not be repaired.  Be mindful of that.

One way to recognize an OS dependency is when an OS specific DLL file such as `KERNEL32.dll` is mentioned in an error.
