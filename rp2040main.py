import time

from machine import UART, Pin, Timer

import api
import zoom

LED = Pin(22, Pin.OUT)

uart = UART(1, baudrate=115200, stop=2, tx=Pin(4), rx=Pin(5), timeout=50)
RS422Pin = Pin(2, Pin.OUT)
RS422Pin.on()


class Status:
    Brightness = len(api.CONTRAST) - 1
    Contrast = len(api.BRIGHTNESS) - 1
    Mide = len(api.MIDE) - 1
    Zoom = zoom.FOV.WFOV
    Polarity = zoom.ACT1.POLARITY_WH


class Button:
    Brightness = Pin(6, Pin.IN, Pin.PULL_UP)
    Contrast = Pin(7, Pin.IN, Pin.PULL_UP)
    Mide = Pin(8, Pin.IN, Pin.PULL_UP)
    Inversion = Pin(9, Pin.IN, Pin.PULL_UP)

    ZoomIn = Pin(10, Pin.IN, Pin.PULL_UP)  # plus
    ZoomOut = Pin(11, Pin.IN, Pin.PULL_UP)  # minus
    # ZoomIn = Pin(11, Pin.IN, Pin.PULL_UP)
    # ZoomOut = Pin(10, Pin.IN, Pin.PULL_UP)

    Calibration = Pin(12, Pin.IN, Pin.PULL_UP)
    AutoFocus = Pin(13, Pin.IN, Pin.PULL_UP)

    FocusIn = Pin(14, Pin.IN, Pin.PULL_UP)
    FocusOut = Pin(15, Pin.IN, Pin.PULL_UP)


class ButtonState:
    Brightness = 0
    Contrast = 0
    Mide = 0
    Polarity = 0
    AutoFocus = 0
    ZoomIn = 0
    ZoomOut = 0
    Calibration = 0
    FocusIn = 0
    FocusOut = 0
    FocusStop = 1


def send_command(command: list[int]):
    ret = uart.write(bytes(command))
    print(f"UART: bytes sent: {ret}")


def handle_contrast_btn():
    if Button.Contrast.value():
        ButtonState.Contrast = 0
    elif ButtonState.Contrast != 1:
        ButtonState.Contrast = 1
        Status.Contrast += 1
        if Status.Contrast == len(api.CONTRAST):
            Status.Contrast = 0
        send_command(api.CONTRAST[Status.Contrast])
        print(f"{Status.Contrast=}")


def handle_brightness_btn():
    if Button.Brightness.value():
        ButtonState.Brightness = 0
    elif ButtonState.Brightness != 1:
        ButtonState.Brightness = 1
        Status.Brightness += 1
        if Status.Brightness == len(api.BRIGHTNESS):
            Status.Brightness = 0
        send_command(api.BRIGHTNESS[Status.Brightness])
        print(f"{Status.Brightness=}")


def handle_mide_btn():
    if Button.Mide.value():
        ButtonState.Mide = 0
    elif ButtonState.Mide != 1:
        ButtonState.Mide = 1
        Status.Mide += 1
        if Status.Mide == len(api.MIDE):
            Status.Mide = 0
        send_command(api.MIDE[Status.Mide])
        print(f"{Status.Mide=}")


def send_zoom():
    send_command(zoom.compile_zoom_only(Status.Polarity, Status.Zoom))
    print(f"{Status.Zoom=}")


def handle_zoomin_btn():
    if Button.ZoomIn.value():
        ButtonState.ZoomIn = 0
    elif ButtonState.ZoomIn != 1:
        ButtonState.ZoomIn = 1
        if Status.Zoom != zoom.FOV.NFOV:
            if Status.Zoom == zoom.FOV.WFOV:
                Status.Zoom = zoom.FOV.MFOV
            elif Status.Zoom == zoom.FOV.MFOV:
                Status.Zoom = zoom.FOV.NFOV
            send_zoom()


def handle_zoomout_btn():
    if Button.ZoomOut.value():
        ButtonState.ZoomOut = 0
    elif ButtonState.ZoomOut != 1:
        ButtonState.ZoomOut = 1
        if Status.Zoom != zoom.FOV.WFOV:
            if Status.Zoom == zoom.FOV.NFOV:
                Status.Zoom = zoom.FOV.MFOV
            elif Status.Zoom == zoom.FOV.MFOV:
                Status.Zoom = zoom.FOV.WFOV
            send_zoom()


def handle_calibration_btn():
    if Button.Calibration.value():
        ButtonState.Calibration = 0
    elif ButtonState.Calibration != 1:
        ButtonState.Calibration = 1
        send_command(zoom.compile_calibration(Status.Polarity, Status.Zoom))
        print("Calibration")


def handle_autofocus_btn():
    if Button.AutoFocus.value():
        ButtonState.AutoFocus = 0
    elif ButtonState.AutoFocus != 1:
        ButtonState.AutoFocus = 1
        send_command(api.AutoFocus)
        print("AutoFocus")


def handle_focusin_btn():
    if Button.FocusIn.value():
        ButtonState.FocusIn = 0
    elif ButtonState.FocusIn != 1:
        ButtonState.FocusIn = 1
        send_command(api.FocusIn)
        print("FocusIn")


def handle_focusout_btn():
    if Button.FocusOut.value():
        ButtonState.FocusOut = 0
    elif ButtonState.FocusOut != 1:
        ButtonState.FocusOut = 1
        send_command(api.FocusOut)
        print("FocusOut")


def handle_foucusstop():
    if Button.FocusOut.value() and Button.FocusIn.value():
        if ButtonState.FocusStop == 0:
            ButtonState.FocusStop = 1
            send_command(api.FocusStop)
            print("FocusStop")
    elif ButtonState.FocusStop == 1:
        ButtonState.FocusStop = 0


def handle_inversion_btn():
    if Button.Inversion.value():
        ButtonState.Polarity = 0
    elif ButtonState.Polarity != 1:
        ButtonState.Polarity = 1
        if Status.Polarity == zoom.ACT1.POLARITY_WH:
            Status.Polarity = zoom.ACT1.POLARITY_BH
        else:
            Status.Polarity = zoom.ACT1.POLARITY_WH
        send_command(zoom.compile_polarity_only(Status.Polarity))
        print(f"{Status.Polarity=}")


def get_status_callback(timer):
    try:
        get_status()
    except Exception as e:
        print(f"Error in status timer: {e}")


# def get_status():
#     print("Ping...")
#     send_command(api.GET_STATUS)
#
#     # Attempt to read the correct 2-byte header
#     for retry in range(10):
#         h = uart.read(2)
#         if h == b'\x10\x02':
#             break
#         print(f"Retry {retry + 1}: {h}")
#     else:
#         print("Header not found after retries")
#         return
#
#     try:
#         data = uart.read(20)
#         if not data or len(data) < 20:
#             raise ValueError(f"Incomplete data: {data}")
#         ret = h + data
#         print(f"Received: {ret}")
#         api.parse_status_response(ret)
#     except Exception as e:
#         print(f"Failed to get status: {e}")


def get_status():
    print("Ping...")
    send_command(api.GET_STATUS)

    # Expecting 2-byte header + 20-byte body
    expected_len = 22
    data = uart.read(expected_len)

    if not data or len(data) < expected_len:
        print(f"Timeout or incomplete data: {data}")
        return

    if data[:2] != b'\x10\x02':
        print(f"Invalid header: {data[:2]}")
        return

    try:
        print(f"Received: {data}")
        api.parse_status_response(data)
    except Exception as e:
        print(f"Failed to parse status: {e}")


def main():
    status_timer = Timer(-1)
    status_timer.init(period=200, mode=Timer.PERIODIC, callback=get_status_callback)
    try:
        while True:
            time.sleep_ms(100)
            LED.value(int(not LED.value()))

            handle_brightness_btn()
            handle_contrast_btn()
            handle_mide_btn()

            handle_zoomin_btn()
            handle_zoomout_btn()

            handle_autofocus_btn()
            handle_focusin_btn()
            handle_focusout_btn()
            handle_foucusstop()

            handle_calibration_btn()
            handle_inversion_btn()
    except KeyboardInterrupt:
        print("Interrupted by user.")
    finally:
        status_timer.deinit()
        print("Timer stopped.")

main()
