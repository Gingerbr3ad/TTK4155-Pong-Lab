from PIL import Image
from pathlib import Path

file_name = "spring_4"

image_path = Path("scripts/sprites/", f"{file_name}.png")
array_path = Path("scripts/sprites/", f"{file_name}.h")
image = Image.open(image_path).convert("L")

width, height = image.size

# Creates a bitmap for the sprite
# A bitmap is a packed array of bits in which each byte represents 8 pixels
bitmap = []
for band in range((height + 7) // 8): # Divide the sprite into 8 bit bands
    for x in range(width):
        column = 0
        for bit in range(8):
            y = band * 8 + bit
            if y < height:
                pixel = image.getpixel((x, y))
                if pixel < 128:  # Cutoff at 128 (half) luminance
                    column |= (1 << bit) # Adds the pixel bit to the bitmap byte
        bitmap.append(column)

# Write the sprite bitmap to a C header and c files
with open(array_path, "w") as file:
    file.write(f"############################## COPY TO .h ############################## \n")
    
    file.write(f"#define {file_name.upper()}_WIDTH {width}\n")
    file.write(f"#define {file_name.upper()}_HEIGHT {height}\n\n")

    file.write(f"extern const uint8_t PROGMEM {file_name}[{len(bitmap)}];\n\n")

    file.write(f"############################## COPY TO .c ############################## \n")

    file.write(f"const uint8_t PROGMEM {file_name}[{len(bitmap)}] = {{\n")

    for i in range(0, len(bitmap), width):
        line = "    " # Indentation at each new line
        for j in range(i, i + width):
            line += f"0x{bitmap[j]:02X}, "
        file.write(line + "\n")
    file.write("};\n")

print(f"Sprite bitmap created succesfully")
