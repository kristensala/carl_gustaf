#+feature dynamic-literals
package main

import "base:runtime"
import win "core:sys/windows"
import "core:strings"
import "core:fmt"

window_size : [2]i32 = {1000, 300}

LIST_ITEM_HEIGHT :: 25

default_applications := map[string]string{
    "Terminal" = "wt.exe",
    "Task Manager" = "taskmgr.exe",
    "Calculator" = "calc.exe",
    "File explorer" = "explorer.exe",
    "Control Panel" = "control.exe",
    "Settings" = "ms-settings:home",
    "Settings: Display" = "ms-settings:display",
    "Settings: Sound" = "ms-settings:sound",
    "Settings: Bluetooth" = "ms-settings:bluetooth",
    "Settings: System" = "ms-settings:system",
}

State :: struct {
    applications: [dynamic]List_Item,

    rect: win.RECT,

    search_phrase: [256]win.WCHAR,
    search_phrase_len: i32, 
    search_results: [dynamic]^List_Item,


    selected_application_idx: i32,
}

List_Item :: struct {
    rect: win.RECT,
    application_name: string,
    command: string
}

destroy_state :: proc(state: ^State) {
    delete(state.applications)

    free(state)
}

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


    state := new(State)

    pos : [2]i32
    center_window(&pos, window_size)

	hwnd := win.CreateWindowW(
        window_class_name,
		win.L("Launcher"),
		win.WS_POPUPWINDOW, 
		pos.x, pos.y, window_size.x, window_size.y,
		nil, nil, instance, state)
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

    destroy_state(state)

}

set_state :: #force_inline proc(hwnd: win.HWND, app: ^State) {win.SetWindowLongPtrW(hwnd, win.GWLP_USERDATA, win.LONG_PTR(uintptr(app)))}
get_state:: #force_inline proc(hwnd: win.HWND) -> ^State {return (^State)(rawptr(uintptr(win.GetWindowLongPtrW(hwnd, win.GWLP_USERDATA))))}

win_proc :: proc "stdcall" (hwnd: win.HWND, msg: win.UINT, wparam: win.WPARAM, lparam: win.LPARAM) -> win.LRESULT {
	context = runtime.default_context()

	switch(msg) {
    case win.WM_CREATE: wm_create(hwnd, lparam)
    case win.WM_KEYDOWN: wm_keydown(hwnd, wparam)
	case win.WM_PAINT: wm_paint(hwnd)
    case win.WM_CHAR:
        state := get_state(hwnd)
        character := win.WCHAR(wparam)

        switch character {
        case 8: // Backspace
            if state.search_phrase_len > 0 {
                state.selected_application_idx = 0
                state.search_phrase_len -= 1
                state.search_phrase[state.search_phrase_len] = 0
            }
        case:
            // Ignore control characters.
            if character >= 32 && state.search_phrase_len < len(state.search_phrase)-1 {
                state.selected_application_idx = 0

                state.search_phrase[state.search_phrase_len] = character
                state.search_phrase_len += 1
                state.search_phrase[state.search_phrase_len] = 0

                clear(&state.search_results)
                pos_y : i32 = 35
                for &item in state.applications {
                    item.rect = {}
                    phrase := win.utf16_to_utf8(state.search_phrase[:state.search_phrase_len]) or_else ""

                    phrase_lower := strings.to_lower(phrase, context.temp_allocator)
                    application_name_lower := strings.to_lower(item.application_name, context.temp_allocator)

                    if strings.contains(application_name_lower, phrase_lower) {
                        item.rect = win.RECT{
                            left = 0,
                            top = pos_y,
                            right = window_size.x,
                            bottom = pos_y + 20
                        }
                        pos_y += 25

                        append(&state.search_results, &item)
                    }
                }
                win.InvalidateRect(hwnd, &state.rect, win.TRUE)

            }
        }

        win.InvalidateRect(hwnd, nil, win.FALSE)
        return 0
	case win.WM_DESTROY:
        win.PostQuitMessage(0)
        return 0
	}

	return win.DefWindowProcW(hwnd, msg, wparam, lparam)
}

wm_keydown :: proc(hwnd: win.HWND, wparam: win.WPARAM) -> win.LRESULT {
    state := get_state(hwnd)
    if wparam == win.VK_ESCAPE {
        win.PostMessageA(hwnd, win.WM_CLOSE, 0, 0);
        return 0
    }

    if wparam == win.VK_UP && len(state.search_results) > 0 {
        old := state.search_results[state.selected_application_idx]
        win.InvalidateRect(hwnd, &old.rect, win.FALSE)

        if state.selected_application_idx == 0 {
            state.selected_application_idx = i32(len(state.search_results)) - 1
        } else {
            state.selected_application_idx -= 1
        }

        new := state.search_results[state.selected_application_idx]
        win.InvalidateRect(hwnd, &new.rect, win.FALSE)
        return 0
    }

    if wparam == win.VK_DOWN && len(state.search_results) > 0 {
        old := state.search_results[state.selected_application_idx]
        win.InvalidateRect(hwnd, &old.rect, win.FALSE)

        state.selected_application_idx += 1
        if state.selected_application_idx >= i32(len(state.search_results)) {
            state.selected_application_idx = 0
        } 

        new := state.search_results[state.selected_application_idx]
        win.InvalidateRect(hwnd, &new.rect, win.FALSE)

        return 0
    }

    if wparam == win.VK_RETURN {
        selected_program := state.search_results[state.selected_application_idx]

        shell_exec(hwnd, selected_program^)
        return 0

    }

    return 0
}

wm_create :: proc(hwnd: win.HWND, lparam: win.LPARAM) -> win.LRESULT {
    pcs := (^win.CREATESTRUCTW)(rawptr(uintptr(lparam)))
    assert(pcs != nil)

    state := (^State)(pcs.lpCreateParams)
    assert(state != nil)

    set_state(hwnd, state)
    win.GetClientRect(hwnd, &state.rect)

    pos_y : i32 = 35
    for name, command in default_applications {
        rc := win.RECT{
            left = 0,
            top = pos_y,
            right = window_size.x,
            bottom = pos_y + 20
        }
        list_item := List_Item{
            rc,
            name,
            command
        }
        append(&state.applications, list_item)

        pos_y += 25
    }

    for &x in state.applications {
        append(&state.search_results, &x)
    }


    return 0
}

wm_paint :: proc(hwnd: win.HWND) -> win.LRESULT {
    state := get_state(hwnd)
    assert(state != nil)

    ps: win.PAINTSTRUCT
    hdc := win.BeginPaint(hwnd, &ps)

    win.FillRect(
        hdc,
        &state.rect,
        win.GetSysColorBrush(win.COLOR_WINDOW),
    )

    rect := win.RECT{0,5,1000,30}
    win.FillRect(hdc, &rect, win.GetSysColorBrush(win.COLOR_WINDOW))
    win.DrawTextW(
        hdc,
        cstring16(&state.search_phrase[0]),
        -1,
        &rect,
        .DT_WORDBREAK,
    )

    fmt.println("draw: ", state.search_results)
    for item, idx in state.search_results {
        name: cstring16 = win.utf8_to_wstring(item.application_name)

        if state.selected_application_idx == i32(idx) {
            win.FillRect(hdc, &item.rect, win.GetSysColorBrush(win.BLACK_BRUSH))
            brush := win.CreateSolidBrush(win.RGB(40, 80, 160))
            win.FillRect(hdc, &item.rect, brush)
            win.DeleteObject(win.HGDIOBJ(brush))

            win.SetBkMode(hdc, .TRANSPARENT)
            win.SetTextColor(hdc, win.RGB(255, 255, 255))
        } else {
            win.FillRect(hdc, &item.rect, win.GetSysColorBrush(win.NULL_BRUSH))
            win.SetTextColor(hdc, win.RGB(0, 0, 0))
        }

        win.DrawTextW(
            hdc,
            name,
            win.INT(len(name)),
            &item.rect,
            .DT_WORDBREAK,
        )
    }


    win.EndPaint(hwnd, &ps)
    return 0
}

center_window :: proc(position: ^[2]i32, size: [2]i32) {
	if deviceMode: win.DEVMODEW; win.EnumDisplaySettingsW(nil, win.ENUM_CURRENT_SETTINGS, &deviceMode) {
		device_size : [2]i32 = {i32(deviceMode.dmPelsWidth), i32(deviceMode.dmPelsHeight)}
		position^ = (device_size - size) / 2
	}
}

shell_exec :: proc(hwnd: win.HWND, item: List_Item) {
    startup := win.STARTUPINFOW{}
    startup.cb = win.DWORD(size_of(startup))

    process: win.PROCESS_INFORMATION

    command: cstring16 = win.utf8_to_wstring(item.command)
    _ = win.ShellExecuteW(
        nil,
        win.L("open"),
        command,
        nil,
        nil,
        win.SW_SHOWNORMAL,
    )


    // These handles are no longer needed if you will not wait for the program.
    win.CloseHandle(process.hThread)
    win.CloseHandle(process.hProcess)

    // close launcher after
    win.PostMessageA(hwnd, win.WM_CLOSE, 0, 0)
}
