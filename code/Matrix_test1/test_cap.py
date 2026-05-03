from PIL import Image
import math

img = Image.open('grad_cap/image.png').convert('L')
if img.size != (15, 15):
    img = img.resize((15, 15))

pixels = list(img.getdata())
print("Max pixel:", max(pixels))
print("Min pixel:", min(pixels))

downsampled = []
for i in range(5):
    for j in range(5):
        block_sum = 0
        for bi in range(3):
            for bj in range(3):
                y = i*3 + bi
                x = j*3 + bj
                block_sum += pixels[y*15 + x]
        downsampled.append(block_sum / 9.0)

max_val = max(downsampled)
min_val = min(downsampled)

stretched = [int(round((v - min_val) / (max_val - min_val) * 100)) for v in downsampled]
print("Stretched downsampled array:")
print(','.join(map(str, stretched)))

thresholded = [v if v > 30 else 0 for v in stretched]
print(f"Thresholded (> 30):")
print(','.join(map(str, thresholded)))

thresholded40 = [v if v > 40 else 0 for v in stretched]
print(f"Thresholded (> 40):")
print(','.join(map(str, thresholded40)))

# Print ASCII of original image to see what the image actually looks like!
print("Original Image Ascii:")
for i in range(15):
    row_chars = []
    for j in range(15):
        val = pixels[i*15 + j]
        row_chars.append('#' if val > 127 else '.')
    print(''.join(row_chars))

