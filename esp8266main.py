import time
from machine import UART, Pin
import api

# pins definition

# LED = Pin(2, Pin.OUT) # GPIO2

uart = UART(1, baudrate=115200)
# RS422Pin = Pin(15, Pin.OUT)
# RS422Pin.on()


class Status:
    Brightness = 0
    Contrast = 0
    Mide = 0
    Zoom = 0
    Inversion = api.WhiteHot


class Button:
    Brightness = Pin(12, Pin.IN, Pin.PULL_UP)
    Contrast = Pin(14, Pin.IN, Pin.PULL_UP)
    # Mide = Pin(8, Pin.IN, Pin.PULL_UP)
    Inversion = Pin(0, Pin.IN, Pin.PULL_UP)
    # AutoFocus = Pin(4, Pin.IN, Pin.PULL_UP)
    # ZoomIn = Pin(5, Pin.IN, Pin.PULL_UP)
    # ZoomOut = Pin(6, Pin.IN, Pin.PULL_UP)
    # Calibration = Pin(9, Pin.IN, Pin.PULL_UP)
    # FocusIn = Pin(10, Pin.IN, Pin.PULL_UP)
    # FocusOut = Pin(11, Pin.IN, Pin.PULL_UP)


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


def send_command(command: list[int]):
    uart.write(bytes(command))


def focus_idle(button):
    while not button.value():
        time.sleep_ms(50)
    send_command(api.FocusStop)


def handle_contrast_btn():
    if Button.Contrast.value():
        ButtonState.Contrast = 0
    elif ButtonState.Contrast != 1:
        ButtonState.Contrast = 1
        send_command(api.CONTRAST[Status.Contrast + 1])
        Status.Contrast += 1
        if Status.Contrast == len(api.CONTRAST):
            Status.Contrast = 0


def handle_brightness_btn():
    if Button.Brightness.value():
        ButtonState.Brightness = 0
    elif ButtonState.Brightness != 1:
        ButtonState.Brightness = 1
        send_command(api.BRIGHTNESS[Status.Brightness + 1])
        Status.Brightness += 1
        if Status.Brightness == len(api.BRIGHTNESS):
            Status.Brightness = 0


def handle_mide_btn():
    if Button.Mide.value():
        ButtonState.Mide = 0
    if ButtonState.Mide != 1:
        ButtonState.Mide = 1
        send_command(api.MIDE[Status.Mide + 1])
        Status.Mide += 1
        if Status.Mide == len(api.MIDE):
            Status.Mide = 0


def handle_zoomin_btn():
    if Button.ZoomIn.value():
        ButtonState.ZoomIn = 0
    if ButtonState.ZoomIn != 1:
        ButtonState.ZoomIn = 1
        send_command(api.Zoom[Status.Zoom + 1])
        if Status.Zoom + 1 < len(api.Zoom):
            Status.Zoom += 1



def handle_zoomout_btn():
    if Button.ZoomOut.value():
        ButtonState.ZoomOut = 0
    if ButtonState.ZoomOut != 1:
        ButtonState.ZoomOut = 1
        send_command(api.Zoom[Status.Zoom - 1])
        if Status.Zoom > 0:
            Status.Zoom -= 1


def handle_calibration_btn():
    if Button.Calibration.value():
        ButtonState.Calibration = 0
    elif ButtonState.Calibration != 1:
        ButtonState.Calibration = 1
        send_command(api.Calibration)


def handle_autofocus_btn():
    if Button.AutoFocus.value():
        ButtonState.AutoFocus = 0
    elif ButtonState.AutoFocus != 1:
        ButtonState.AutoFocus = 1
        send_command(api.AutoFocus)


def handle_focusin_btn():
    # elif not Button.FocusIn.value():
    #     send_command(api.FocusIn)
    #     focus_idle(Button.FocusIn)
    ...


def handle_focusout_btn():
    # elif not Button.FocusOut.value():
    #     send_command(api.FocusOut)
    #     focus_idle(Button.FocusOut)
    ...


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


def main():
    while True:
        time.sleep_ms(100)

        handle_brightness_btn()
        handle_contrast_btn()
        handle_mide_btn()

        handle_zoomin_btn()
        handle_zoomout_btn()

        handle_autofocus_btn()
        handle_focusin_btn()
        handle_focusout_btn()

        handle_calibration_btn()
        handle_inversion_btn()


main()