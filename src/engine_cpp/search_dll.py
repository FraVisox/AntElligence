import os
import ctypes

dll_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "engine.dll")
print(f"Looking for DLL at: {dll_path}")
print(f"File exists: {os.path.exists(dll_path)}")

if os.path.exists(dll_path):
    print(f"File size: {os.path.getsize(dll_path)} bytes")
    try:
        dll = ctypes.WinDLL(dll_path)
        print("Successfully loaded DLL!")
        print(f"Available functions: {dir(dll)}")
    except Exception as e:
        print(f"Failed to load DLL: {e}")