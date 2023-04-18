# logo: https://www.aigei.com/icon/class/bluetooth?tab=file

# 图片二值化
from PIL import Image
from pathlib import Path


def get_xbmp(path, resize=(16, 16), mode="LSB", threshold=150):
    img = Image.open(path)
    # print(img.format)

    if img.format == "PNG":
        img = img.convert("RGBA")

        # if img.mode != "RGBA":
        #     img = img.convert("RGBA")

        width = img.width
        height = img.height

        img2 = Image.new("RGB", size=(width, height), color=(255, 255, 255))
        img2.paste(img, (0, 0), mask=img)
        img = img2

    img = img.convert("L")  # 灰度化

    # img2.save("test.png")
    # return

    # img = img.convert("1")
    table = []
    for i in range(256):
        if i < threshold:
            table.append(1)
        else:
            table.append(0)
    img = img.point(table, "1")  # 图片二值化

    img = img.resize(resize)
    # img.save("test.png")
    img.save(Path("./test/").joinpath(path.name))

    buffers = []
    ptr = 0
    buffer = 0
    for data in list(img.getdata()):
        if mode == "MSB":
            buffer = buffer << 1
            buffer = buffer | data
        elif mode == "LSB":
            buffer = buffer >> 1
            buffer = buffer | (data << 7)

        ptr += 1
        if ptr == 8:
            buffers.append(buffer)
            buffer = 0
            ptr = 0
    return buffers


def format_output(filename, buffers):
    print(
        "const unsigned char %s[] U8X8_PROGMEM = {"
        % filename.replace("-", "_").upper(),
        end="",
    )
    for i, data in enumerate(buffers):
        if i % 8 == 0:
            print("\n\t", end="")
        print("{:#04x}".format(data), end=", ")
    print("\n};")


# paths = Path("./logo/").rglob("*")
# for path in paths:
#     buffers = get_xbmp(path)
#     format_output(path.stem, buffers)


path = Path("./logo/success.png")
buffers = get_xbmp(path, resize=(40, 40))
format_output(path.stem, buffers)

path = Path("./logo/wheel.png")
buffers = get_xbmp(path, resize=(8, 8))
format_output(path.stem, buffers)

path = Path("./logo/mouse.png")
buffers = get_xbmp(path, resize=(8, 8))
format_output(path.stem, buffers)
