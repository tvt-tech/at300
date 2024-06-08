def crc16(crc, data):
    crc = crc ^ (data & 0x00FF)
    for _ in range(8):
        if crc & 1:
            crc = (crc >> 1) ^ 0xA001
        else:
            crc = crc >> 1
    return crc


def crc16fast(crc, data):
    crctable = [
        0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
        0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
    ]
    crc = crc ^ (data & 0x00FF)
    crc = (crc >> 4) ^ crctable[crc & 0xF]
    crc = (crc >> 4) ^ crctable[crc & 0xF]
    return crc


def format_bytes(data):
    return ''.join([f"\\x{i:02X}" for i in data])


def senddex(string):
    ret = bytearray()

    crc = 0x0000
    print("0x10 - DLE")
    ret.append(0x10)
    # print("0x01 - SOH")
    # ret.append(0x01)
    print("0x02 - SOH")
    ret.append(0x02)
    for char in string:
        # print(f"0x{ord(char):02X} - {char}")
        # crc = crc16fast(crc, ord(char))
        print(f"0x{char:02X} - {char}")
        ret.append(char)
        crc = crc16fast(crc, char)
    print("0x10 - DLE")
    ret.append(0x10)
    print("0x03 - ETX")
    ret.append(0x03)
    crc = crc16fast(crc, 0x03)
    print("0x{:02X} - CRC Low".format((crc >> 0) & 0x00FF))
    ret.append((crc >> 0) & 0x00FF)
    print("0x{:02X} - CRC High".format((crc >> 8) & 0x00FF))
    ret.append((crc >> 8) & 0x00FF)
    # print("Actual  ", format_bytes(ret))


    # DLE, SOH, *DATA, DLE_, ETX, CRC_LO, CRC_HI = ret
    # data = (DLE, SOH, DATA, DLE_, ETX, CRC_LO, CRC_HI)
    # keys = ('DLE', 'SOH', 'DATA', 'DLE_', 'ETX', 'CRC Low', 'CRC High')
    # data_dict = dict(zip(keys, data))
    #
    # for k, v in data_dict.items():
    #     if isinstance(v, int):
    #         print(f"0x{v:02X} - {k}")
    #     elif isinstance(v, (list, bytes, bytearray)):
    #         for b in v:
    #             print(f"0x{b:02X} - {b}")

    return ret


# crc16tab = [0] * 256


def CRC_table():
    crc16tab = [0] * 256
    for h in range(256):
        r = h << 8
        for k in range(8):
            if r & 0x8000:
                r = (r << 1) ^ 0x1021
            else:
                r = r << 1
        crc16tab[h] = r
    return crc16tab


CRCtable = CRC_table()

def crc16_ccitt(buf):
    crc16tab = CRC_table()
    crc = 0x0
    for byte in buf:
        crc = crc16tab[((crc >> 8) ^ byte) & 0xFF] ^ (crc << 8)
    return crc & 0xFFFF


def crc16_ccitt_fast(data_buffer):
    crc = 0x0
    for byte in data_buffer:
        crc = CRCtable[((crc >> 8) ^ byte) & 0xFF] ^ (crc << 8)
    return crc & 0xFFFF


def checksum_message(msg: list[int]):
    return bytes(msg[0:-2]) + crc16_ccitt_fast(msg[2:-4]).to_bytes(2, byteorder='big')


def int16_to_bytes(value):
    if not (0 <= value <= 0xFFFF):
        raise ValueError("Value must be a 2-byte integer (0 <= value <= 65535)")
    byte1 = (value >> 8) & 0xFF  # Extract the higher byte
    byte2 = value & 0xFF  # Extract the lower byte

    return bytearray([byte1, byte2])


def ccitt_message(msg: list[int]):
    # return bytes(msg[0:-2]) + crc16_ccitt(msg[2:-4]).to_bytes(2, byteorder='big')
    return bytes(msg[0:-2]) + int16_to_bytes(crc16_ccitt(msg[2:-4]))



# if __name__ == "__main__":
#     import api
#     from api import WhiteHot
#     EXP = bytes(WhiteHot[:2]) + bytes(WhiteHot[2:-2]) + bytes(WhiteHot[-2:])
#     CRC = checksum_message(WhiteHot)
#     CCIT = ccitt_message(WhiteHot)
#     ACT = senddex(WhiteHot[2:-4])
#     print("Actual  ", format_bytes(ACT))
#     print("Expected", format_bytes(EXP))
#     print("Checksum", format_bytes(CRC))
#     print("CCIT    ", format_bytes(CCIT))
#     print(EXP == CRC == CCIT)
#
#
#     print(checksum_message(api.Calibration))