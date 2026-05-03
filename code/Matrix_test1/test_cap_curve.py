v_list = [
    1,30,77,29,0,
    59,92,94,100,57,
    11,72,92,84,7,
    11,60,95,45,0,
    11,17,13,0,0
]

def apply_curve(val, power):
    # Normalize to 0-1
    norm = val / 100.0
    # Apply power
    curved = norm ** power
    # Scale back to 100
    res = int(round(curved * 100))
    # Threshold at some level to remove noise? (user mentioned previously thresholding to 30)
    # Let's say if it's < 15, just set to 0.
    return res if res > 5 else 0

print("Quadratic:")
quad = [apply_curve(v, 2) for v in v_list]
print(quad)

print("Cubic:")
cubic = [apply_curve(v, 3) for v in v_list]
print(cubic)

print("Quartic (4th power):")
quartic = [apply_curve(v, 4) for v in v_list]
print(quartic)
