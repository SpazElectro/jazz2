import win32api, win32gui, time
import ctypes

class MENUINFO(ctypes.Structure):
    _fields_ = [("cbSize", ctypes.c_uint),
                ("fMask", ctypes.c_uint),
                ("dwStyle", ctypes.c_uint),
                ("cyMax", ctypes.c_uint),
                ("hbrBack", ctypes.c_void_p),
                ("dwContextHelpID", ctypes.c_uint),
                ("dwMenuData", ctypes.c_void_p)]

hwnd = win32gui.FindWindow(None, "Jazz Jackrabbit 2 - Easter Bunny")

print(hwnd)

menu = win32gui.CreatePopupMenu()
win32gui.AppendMenu(menu, 0, 6, "awesome minus")

mi = MENUINFO()
mi.fMask = 16
mi.dwStyle = 0x40000000
mi.cbSize = ctypes.sizeof(MENUINFO)

while True:
    win32gui.SetMenuInfo(win32gui.GetMenu(hwnd), mi)
    win32gui.DrawMenuBar(hwnd)
    time.sleep(0.001)