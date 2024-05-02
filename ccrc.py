from api import Zoom


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
    print("Actual  ", format_bytes(ret))
    return ret


if __name__ == "__main__":
    senddex(Zoom[0][2:-4])
    print("Expected", format_bytes(
        bytes(Zoom[0][:2]) + bytes(Zoom[0][2:-2]) + bytes(Zoom[0][-2:])
    ))
