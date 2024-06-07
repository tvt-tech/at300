
import time

from machine import UART, Pin

import api
import zoom

LED = Pin(25, Pin.OUT)

uart = UART(1, baudrate=115200, stop=2, tx=Pin(4), rx=Pin(5))
RS422Pin = Pin(2, Pin.OUT)
RS422Pin.on()


class Status:
    Brightness = len(api.CONTRAST) - 1
    Contrast = len(api.BRIGHTNESS) - 1
    Mide = len(api.MIDE) - 1
    Zoom = len(api.ZoomWH) - 1
    Inversion = api.WhiteHot


class Button:
    Brightness = Pin(6, Pin.IN, Pin.PULL_UP)
    Contrast = Pin(7, Pin.IN, Pin.PULL_UP)
    Mide = Pin(8, Pin.IN, Pin.PULL_UP)
    Inversion = Pin(9, Pin.IN, Pin.PULL_UP)

    ZoomIn = Pin(10, Pin.IN, Pin.PULL_UP)
    ZoomOut = Pin(11, Pin.IN, Pin.PULL_UP)
    Calibration = Pin(12, Pin.IN, Pin.PULL_UP)
    AutoFocus = Pin(13, Pin.IN, Pin.PULL_UP)

    FocusIn = Pin(14, Pin.IN, Pin.PULL_UP)
    FocusOut = Pin(15, Pin.IN, Pin.PULL_UP)


class ButtonState:
    Brightness = 0
    Contrast = 0
    Mide = 0
    Inversion = 0
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


def handle_zoomin_btn():
    if Button.ZoomIn.value():
        ButtonState.ZoomIn = 0
    elif ButtonState.ZoomIn != 1:
        ButtonState.ZoomIn = 1
        if Status.Zoom + 1 < len(api.ZoomWH):
            Status.Zoom += 1
            if Status.Inversion == api.BlackHot:
                send_command(api.ZoomBH[Status.Zoom])
            else:
                send_command(api.ZoomWH[Status.Zoom])
        print(f"{Status.Zoom=}")


def handle_zoomout_btn():
    if Button.ZoomOut.value():
        ButtonState.ZoomOut = 0
    elif ButtonState.ZoomOut != 1:
        ButtonState.ZoomOut = 1
        if Status.Zoom > 0:
            Status.Zoom -= 1
            if Status.Inversion == api.BlackHot:
                send_command(api.ZoomBH[Status.Zoom])
            else:
                send_command(api.ZoomWH[Status.Zoom])
        print(f"{Status.Zoom=}")


def handle_calibration_btn():
    if Button.Calibration.value():
        ButtonState.Calibration = 0
    elif ButtonState.Calibration != 1:
        ButtonState.Calibration = 1

        if Status.Zoom == 0:
            zoom_ = zoom.FOV.WFOV
        elif Status.Zoom == 1:
            zoom_ = zoom.FOV.MFOV
        else:
            zoom_ = zoom.FOV.NFOV

        if Status.Inversion == api.BlackHot:
            # send_command(api.CalibrationBH)
            polarity_ = zoom.ACT1.POLARITY_BH
        else:
            polarity_ = zoom.ACT1.POLARITY_WH
        send_command(zoom.compile_calibration(polarity_, zoom_))
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
        ButtonState.Inversion = 0
    elif ButtonState.Inversion != 1:
        ButtonState.Inversion = 1
        if Status.Inversion == api.WhiteHot:
            send_command(api.BlackHot)
            Status.Inversion = api.BlackHot
        else:
            send_command(api.WhiteHot)
            Status.Inversion = api.WhiteHot
        print(f"{Status.Inversion=}")



def get_status():
    send_command(api.GET_STATUS)
    retry = 10
    h = uart.read(2)
    while h != b'\x10\x02':
        print(f"Retry: {h}")
        retry -= 1
        if retry <= 0:
            break
    try:
        if h == b'\x10\x02':
            ret = h + uart.read(20)
            print(ret)
            api.parse_status_response(bytes(ret))
            print()
        else:
            print(h)
            raise ValueError("Wrong header")
    except Exception as e:
        print(f"Failed get status: {e}")



def main():
    #get_status_timer = 5000
    while True:
        #get_status_timer -= 100
        #if get_status_timer <= 0:
        #    get_status()
        #    get_status_timer = 5000

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


main()

