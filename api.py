#  USER CODE BEGIN PV
from enum import IntFlag, Enum

from ccrc import ccitt_message, format_bytes

#  ZOOM
Zoom = [
    [0x10, 0x02, 0xF4, 0x02, 0x00, 0x20, 0x00, 0x00, 0x64, 0x10, 0x03, 0x7E, 0x72],
    [0x10, 0x02, 0xF4, 0x02, 0x00, 0x20, 0x00, 0x00, 0xC8, 0x10, 0x03, 0x0A, 0x14],
    [0x10, 0x02, 0xF4, 0x02, 0x00, 0x20, 0x00, 0x01, 0x90, 0x10, 0x03, 0xE2, 0xD8]
]
ZoomTest = [0x10, 0x02, 0xF4, 0x02, 0x01, 0x20, 0x00, 0x03, 0x00, 0x01, 0x90, 0x80, 0x00, 0x00, 0x10, 0x03, 0xC4, 0xD9]

# Calibration=[0x10,0x02,0xF4,0x02,0x01,0x20,0x00,0x03,0x00,0x01,0x90,0x80,0x00,0x00,0x10,0x03,0xE2,0xD8]
Calibration = [0x10, 0x02, 0xF4, 0x02, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00, 0x64, 0x80, 0x00, 0x00, 0x10, 0x03, 0x9C,
               0xC8]

WhiteHot = [0x10, 0x02, 0xF4, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x10, 0x03, 0x6D, 0xAA]
BlackHot = [0x10, 0x02, 0xF4, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x03, 0x65, 0x1E]

# New protocol
FocusIn = [0x10, 0x02, 0x41, 0x02, 0x04, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x10, 0x03, 0x18, 0x08]
FocusOut = [0x10, 0x02, 0x41, 0x02, 0x04, 0x00, 0x20, 0xFF, 0x00, 0x00, 0x10, 0x03, 0x2F, 0x6C]
FocusStop = [0x10, 0x02, 0x41, 0x02, 0x04, 0x00, 0x20, 0x00, 0x00, 0x00, 0x10, 0x03, 0xE0, 0x0F]
AutoFocus = [0x10, 0x02, 0x41, 0x02, 0x04, 0x00, 0x40, 0x00, 0x00, 0x00, 0x10, 0x03, 0xB9, 0xDD]

# # Old protocol
# FocusIn=[0x10,0x02,0x41,0x02,0x00,0x00,0x00,0x60,0x00,0x00,0x10,0x03,0xE2,0xD8]
# FocusOut=[0x10,0x02,0x41,0x02,0x00,0x00,0x00,0x20,0x00,0x00,0x10,0x03,0xE2,0xD8]
# FocusStop=[0x10,0x02,0x41,0x02,0x00,0x00,0x00,0x4A,0x00,0x00,0x10,0x03,0xE2,0xD8]

MIDE = [
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x64, 0xB2],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x0B, 0xF7],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xBA, 0x38],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xD5, 0x7D],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xA6, 0xC2],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x17, 0x0D],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xC9, 0x87],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x78, 0x48],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x2E, 0xF9],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x41, 0xBC],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xF0, 0x73],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x9F, 0x36],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x83, 0xCC],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xEC, 0x89],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x5D, 0x46],
    [0x10, 0x02, 0xF7, 0x02, 0x00, 0x00, 0x00, 0x02, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x32, 0x03]
]

#  Brightness
BRIGHTNESS = [
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x59, 0xB2],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x15, 0x85],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xC1, 0xDC],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x8D, 0xEB],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x79, 0x4F],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0x35, 0x78],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xE1, 0x21],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
     0x00, 0x10, 0x03, 0xAD, 0x16]
]

# BRIGHIT08=[0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]
# BRIGHIT09=[0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xC6,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]
# BRIGHIT10=[0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xDC,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]
# BRIGHIT11=[0x10,0x02,0xF7,0x02,0x42,0x05,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xF2,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]

#  CONTRAST

CONTRAST = [
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0x19, 0x7C],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0x55, 0x4B],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0x81, 0x12],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0xCD, 0x25],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0x39, 0x81],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0x75, 0xB6],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0xA1, 0xEF],
    [0x10, 0x02, 0xF7, 0x02, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x10,
     0x03, 0xED, 0xD8]
]

# CONTR08=[0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]
# CONTR09=[0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]
# CONTR10=[0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDC,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]
# CONTR11=[0x10,0x02,0xF7,0x02,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF2,0x00,0x00,0x00,0x00,0x10,0x03,0xE2,0xD8]

GET_STATUS = (
    0x10, 0x02, 0x5F, 0x02, 0x10, 0x03, 0x3E, 0xC3
)

status_smple1 = (0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10,
                 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x88, 0x10, 0x03, 0xBF, 0x20)
status_smple2 = (0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10,
                 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x88, 0x10, 0x03, 0xBF, 0x20)

status_on = (
    0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x20, 0x10, 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x88, 0x10,
    0x03,
    0xC4, 0x48)
zoom0 = (
    0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x14, 0x00, 0x88, 0x00, 0x88, 0x10,
    0x03,
    0x74, 0xD2)
zoom1 = (
    0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x28, 0x00, 0x88, 0x00, 0x88, 0x10,
    0x03,
    0xF3, 0x17)
zoom2 = (
    0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x88, 0x10,
    0x03,
    0xBF, 0x20)
bh = (
    0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x88, 0x10,
    0x03,
    0xBF, 0x20)
bw = (
    0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x20, 0x10, 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x88, 0x10,
    0x03,
    0xC4, 0x48)


def parse_status_response(buf: bytes):
    DLE = 0x10
    STX = 0x02
    MID = 0x5F
    STM = 0x20
    EXT = 0x03
    BCSH = BCSL = 0xFF  # Depends on message content

    HEADER = (DLE, STX, MID, STM)
    FOOTER = (DLE, EXT)
    CRC = (BCSH, BCSL)

    buf = buf.replace(b'\x10\x10', b'\x10')

    header = buf[:4]
    footer = buf[-4:-2]
    crc = buf[-2:]
    data = buf[4:-4]

    retval = {}

    if header == bytes(HEADER) and footer == bytes(FOOTER):

        (FLT1, FLT2,
         Rsrv1, Rsrv2,
         OPS1, OPS2, OPS3,
         Rsrv3, FOV, OH, OL, BH, BL, *_) = data

        def _bin(b: int):
            return '0b' + bin(b)[2:].zfill(8)

        retval['FLT1'] = hex(FLT1)
        retval['FLT2'] = hex(FLT2)
        # retval['_rsrv'] = Rsrv1
        # retval['_rsrv'] = Rsrv2
        retval['OPS1'] = _bin(OPS1)
        if (OPS2 >> 5) & 0x1:
            retval['Polarity'] = "White Hot"
        else:
            retval['Polarity'] = "Black Hot"
        # retval['OPS3'] = _bin(OPS3)
        if (OPS3 >> 4) & 0x00:
            retval['Orientation'] = "TR2BL"
        if (OPS3 >> 4) & 0x01:
            retval['Orientation'] = "TL2BR"
        if (OPS3 >> 4) & 0x02:
            retval['Orientation'] = "BR2TL"
        if (OPS3 >> 4) & 0x04:
            retval['Orientation'] = "BL2TR"

        # retval['_rsrv'] = Rsrv3
        retval['FOV'] = FOV
        retval['Contrast'] = int.from_bytes(bytes((OL, OH)), 'little')
        retval['Brightness'] = int.from_bytes(bytes((BL, BH)), 'little')

        print("\nGOT STATUS:")
        for k, v in retval.items():
            print(f"\t{k}:\t\t{v}")

        print('\nFAULTS:')

        if (FLT1 >> 3) & 0x1:
            print("\tFLT1:")
            print("\t\tBuilt-In Test fault")

        if FLT2 > 0:
            print("\tFLT2:")
            if (FLT2 >> 0) & 0x1:
                print("\t\tAmbient temperature fault")
            if (FLT2 >> 1) & 0x1:
                print("\t\tFLASH fault")
            if (FLT2 >> 4) & 0x1:
                print("\t\tLens Focus fault")
            if (FLT2 >> 5) & 0x1:
                print("\t\tLens FOV fault")


# parse_status_response(bytes(status_on))
# parse_status_response(bytes(bw))
# parse_status_response(bytes(bh))
# parse_status_response(
#     bytes((0x10, 0x02, 0x5F, 0x20, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x0A, 0x00, 0x88, 0x00, 0x22,
#            0x10, 0x03, 0xAB, 0x80)))


class ACT1(IntFlag):
    NONE = 0
    USE_CTRL_1 = 1 << 0
    POLARITY_WH = 1 << 5  # White Hot
    POLARITY_BH = 0 << 5  # Black Hot


class ACT2(IntFlag):
    NONE = 0
    USE_FOV = 1 << 5


class CTRL1(IntFlag):
    """Calibration method"""
    NONE = 0
    DEFAULT = 0x3 & 0b111


class FOV(Enum):
    NONE = [0x00, 0x00]
    WFOV = [0x00, 0x64]
    MFOV = [0x00, 0xC8]
    NFOV = [0x01, 0x90]


def compile_zoom_command(act1: ACT1 = ACT1.NONE,
                         act2: ACT2 = ACT2.NONE,
                         ctrl1: CTRL1 = CTRL1.NONE,
                         fov: FOV = FOV.WFOV):
    DLE = 0x10
    STX = 0x02
    MID = 0xF4
    STM = 0x02
    Rsrv = 0x00
    CTRL2 = 0x00
    CTRL3 = 0x80
    ETX = 0x03

    header = [DLE, STX, MID, STM]
    footer = [DLE, ETX, Rsrv, Rsrv]

    if act1 & ACT1.USE_CTRL_1:
        body = [act1, act2, Rsrv, ctrl1, CTRL2, *fov.value, CTRL3, Rsrv, Rsrv]
    else:
        body = [act1, act2, Rsrv, *fov.value]
    cmd = bytearray([*header, *body, *footer])
    ret = ccitt_message(cmd)
    print(format_bytes(ret), len(ret))
    return ret


def compile_only_zoom(fov: FOV = FOV.WFOV):
    return compile_zoom_command(act2=ACT2.USE_FOV, fov=fov)


def compile_only_calibration(mode: CTRL1 = CTRL1.DEFAULT):
    return compile_zoom_command(act1=ACT1.USE_CTRL_1, ctrl1=mode, act2=ACT2.USE_FOV, fov=FOV.NFOV)


def compile_only_polarity(polarity: ACT1 = ACT1.POLARITY_WH):
    return compile_zoom_command(act1=polarity)


# compile_zoom_command(act2=ACT2.USE_FOV, fov=FOV.WFOV)
# compile_zoom_command(act1=ACT1.USE_CTRL_1, ctrl1=CTRL1.DEFAULT, fov=FOV.WFOV)
#
# compile_only_calibration(mode=CTRL1.DEFAULT)
# compile_only_polarity(polarity=ACT1.POLARITY_WH)
# compile_only_polarity(polarity=ACT1.POLARITY_BH)
