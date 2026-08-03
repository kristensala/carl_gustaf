package main

import "base:runtime"
import win "core:sys/windows"

window_size : [2]i32 = {1000, 300}

main :: proc() {
	win.SetProcessDPIAware()

	instance := win.HINSTANCE(win.GetModuleHandleW(nil))
	assert(instance != nil, "Failed to fetch current instance")

	window_class_name := cstring16(win.L("carl_gustaf"))
	window_class := win.WNDCLASSW {
		lpfnWndProc = win_proc,
		lpszClassName = window_class_name,
		hInstance = instance,
	}
	class := win.RegisterClassW(&window_class)
	assert(class != 0, "Class creation failed")
    defer win.UnregisterClassW(window_class_name, instance)

    pos : [2]i32
    center_window(&pos, window_size)

	hwnd := win.CreateWindowW(
        window_class_name,
		win.L("Launcher"),
		win.WS_POPUPWINDOW, 
		pos.x, pos.y, window_size.x, window_size.y,
		nil, nil, instance, nil)
	assert(hwnd != nil, "Window creation Failed")


    win.ShowWindow(hwnd, win.SW_SHOWDEFAULT)
    win.SetForegroundWindow(hwnd)
	win.UpdateWindow(hwnd)

    message: win.MSG
    for win.GetMessageW(&message, nil, 0, 0) > 0 {
        win.TranslateMessage(&message)
        win.DispatchMessageW(&message)

        free_all(context.temp_allocator)
    }

}

win_proc :: proc "stdcall" (hwnd: win.HWND, msg: win.UINT, wparam: win.WPARAM, lparam: win.LPARAM) -> win.LRESULT {
	context = runtime.default_context()

	switch(msg) {
    case win.WM_CREATE:
        pcs := (^win.CREATESTRUCTW)(rawptr(uintptr(lparam)))
        return 0
    case win.WM_KEYDOWN:
        if wparam == win.VK_ESCAPE {
            win.PostMessageA(hwnd, win.WM_CLOSE, 0, 0);
            return 0;
        }

	case win.WM_PAINT:
        ps: win.PAINTSTRUCT
        hdc := win.BeginPaint(hwnd, &ps)

        win.EndPaint(hwnd, &ps)
		return 0
	case win.WM_DESTROY:
        win.PostQuitMessage(0)
        return 0
	}

	return win.DefWindowProcW(hwnd, msg, wparam, lparam)
}

center_window :: proc(position: ^[2]i32, size: [2]i32) {
	if deviceMode: win.DEVMODEW; win.EnumDisplaySettingsW(nil, win.ENUM_CURRENT_SETTINGS, &deviceMode) {
		device_size : [2]i32 = {i32(deviceMode.dmPelsWidth), i32(deviceMode.dmPelsHeight)}
		position^ = (device_size - size) / 2
	}
}
