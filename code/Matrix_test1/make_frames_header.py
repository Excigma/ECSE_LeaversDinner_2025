import glob
import os
import sys
import struct

# mkdir -p frames_15x15_subpixel && ffmpeg -i frames/output_%04d.jpg -vf "scale=15:15:flags=bicubic,format=gray" frames_15x15_subpixel/frame_%04d.png -y

SRC_DIR = "frames_15x15_subpixel"
OUT_H = "frames.h"
PATTERN = os.path.join(SRC_DIR, "frame_*.png")
pngs = sorted(glob.glob(PATTERN))
if not pngs:
    print("No frames found in", SRC_DIR)
    sys.exit(1)

def read_png_gray(filepath):
    with open(filepath, 'rb') as f:
        # Check PNG signature
        sig = f.read(8)
        if sig != b'\x89PNG\r\n\x1a\n':
            raise ValueError("Not a PNG file")
        
        width = height = None
        image_data = b''
        
        while True:
            # Read chunk length and type
            chunk_len_bytes = f.read(4)
            if len(chunk_len_bytes) < 4:
                break
            chunk_len = struct.unpack('>I', chunk_len_bytes)[0]
            chunk_type = f.read(4)
            
            if chunk_type == b'IHDR':
                ihdr = f.read(chunk_len)
                width, height = struct.unpack('>II', ihdr[:8])
                bit_depth = ihdr[8]
                color_type = ihdr[9]
                # We expect grayscale (0) or RGB (2), 8-bit
            elif chunk_type == b'IDAT':
                image_data += f.read(chunk_len)
            else:
                f.read(chunk_len)
            
            f.read(4)  # CRC
            
            if chunk_type == b'IEND':
                break
        
        # Decompress IDAT
        import zlib
        raw = zlib.decompress(image_data)
        
        # Parse scanlines (assuming 8-bit grayscale, no filtering for simplicity)
        # Each scanline has 1 filter byte + width bytes
        pixels = []
        bytes_per_row = width + 1  # +1 for filter byte
        
        for y in range(height):
            scanline_start = y * bytes_per_row
            filter_type = raw[scanline_start]
            row_data = raw[scanline_start + 1:scanline_start + bytes_per_row]
            
            # Simple filter handling (type 0 = none)
            if filter_type == 0:
                pixels.extend(row_data)
            elif filter_type == 1:  # Sub filter
                reconstructed = []
                for x in range(width):
                    raw_byte = row_data[x]
                    left = reconstructed[x-1] if x > 0 else 0
                    reconstructed.append((raw_byte + left) & 0xFF)
                pixels.extend(reconstructed)
            else:
                pixels.extend(row_data[:width])
        
        return width, height, pixels

frames = []
for p in pngs:
    try:
        width, height, pixels = read_png_gray(p)
        
        # Subpixel rendering: average 3x3 blocks to get each LED value
        if width == 15 and height == 15:
            output = []
            for led_y in range(5):
                for led_x in range(5):
                    # Sample 3x3 block for this LED
                    block_sum = 0
                    for dy in range(3):
                        for dx in range(3):
                            px_y = led_y * 3 + dy
                            px_x = led_x * 3 + dx
                            idx = px_y * width + px_x
                            block_sum += pixels[idx]
                    # Average the 9 subpixels
                    avg_brightness = block_sum / 9.0
                    # Normalize to 0-1 range
                    normalized = avg_brightness / 255.0
                    
                    # Apply aggressive contrast curve: power of 2.5
                    # This makes dark pixels much darker while keeping bright pixels bright
                    # Also apply a threshold to make very dark pixels completely off
                    if normalized < 0.15:
                        # Below threshold: make it black
                        contrast_adjusted = 0.0
                    else:
                        # Apply power curve with offset compensation
                        # Remap 0.15-1.0 to 0.0-1.0, then apply power curve
                        remapped = (normalized - 0.15) / 0.85
                        contrast_adjusted = pow(remapped, 2.5)
                    
                    output.append(contrast_adjusted)
            
            # Normalize frame so brightest is 1.0 and dimmest is 0.0
            if output:
                min_val = min(output)
                max_val = max(output)
                if max_val > min_val:  # Avoid division by zero
                    # Scale to 0-1 range
                    output = [(v - min_val) / (max_val - min_val) for v in output]
            
            # Convert to 0-100 range
            frames.append([int(round(v * 100.0)) for v in output])
        else:
            print(f"Warning: {p} is {width}x{height}, expected 15x15")
            # Fallback: simple downsample
            output = []
            for led_y in range(5):
                for led_x in range(5):
                    # Just sample center pixel
                    src_y = int(led_y * height / 5.0 + height / 10.0)
                    src_x = int(led_x * width / 5.0 + width / 10.0)
                    idx = src_y * width + src_x
                    output.append(pixels[idx] / 255.0)
            
            # Normalize frame so brightest is 1.0 and dimmest is 0.0
            if output:
                min_val = min(output)
                max_val = max(output)
                if max_val > min_val:
                    output = [(v - min_val) / (max_val - min_val) for v in output]
                else:
                    output = [0.0] * len(output)
            
            # Convert to 0-100 range
            frames.append([int(round(v * 100.0)) for v in output])
    except Exception as e:
        print(f"Error processing {p}: {e}")
        sys.exit(1)

frames_count = len(frames)
w, h = 5, 5

def c_array_initializer(frames):
    lines = []
    for fi, frame in enumerate(frames):
        # format each value as decimal, grouped by row for readability
        rows = []
        for r in range(h):
            row_vals = frame[r*w:(r+1)*w]
            rows.append(", ".join(str(x) for x in row_vals))
        frame_text = ",\n        ".join("{" + row + "}" for row in rows)
        # but we want a flat 25-element initializer per frame
        flat = ", ".join(str(x) for x in frame)
        lines.append("    {" + flat + "}")
    return ",\n".join(lines)

with open(OUT_H, "w") as f:
    f.write("#pragma once\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write(f"#define FRAMES_COUNT {frames_count}\n")
    f.write(f"#define FRAME_W {w}\n")
    f.write(f"#define FRAME_H {h}\n\n")
    f.write("/* frames[frame_index][pixel_index] - row-major, 25 bytes per frame */\n")
    f.write(f"const uint8_t frames[FRAMES_COUNT][{w*h}] = {{\n")
    f.write(c_array_initializer(frames))
    f.write("\n};\n\n")
    f.write("/* Access: frames[frame_idx][y*FRAME_W + x]  -> value 0..100 */\n")

print(f"Wrote {OUT_H} with {frames_count} frames")
