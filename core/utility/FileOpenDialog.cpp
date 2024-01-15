#include "FileOpenDialog.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>

std::string FileOpenDialog::open()
{
    std::string result = "";
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog* pFileOpen;

        // Create the File Open Dialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr)) {
            // Show the File Open Dialog.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the File Open Dialog.
            if (SUCCEEDED(hr)) {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    if (SUCCEEDED(hr)) {
                        // Do something with the selected file (e.g., open and process it)
                        // In this example, we'll print the file path to the console
                        wprintf(L"Selected file: %s\n", pszFilePath);

                        // Convert PWSTR to std::string
                        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, nullptr, 0, nullptr, nullptr);
                        if (bufferSize == 0) {
                            std::cout << "Buffer size == 0\n";
                            exit(-1);
                        }

                        std::string utf8String(bufferSize - 1, '\0');  // Subtract 1 to exclude the null terminator
                        if (WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &utf8String[0], bufferSize, nullptr, nullptr) == 0) {
                            // Handle the error, possibly throw an exception
                            std::cout << "WideCharToMultiByte error\n";
                            exit(-2);
                        }
                        result = utf8String;

                        CoTaskMemFree(pszFilePath);
                    }

                    pItem->Release();
                }
            }

            pFileOpen->Release();
        }

        CoUninitialize();
    }

    return result;
}
