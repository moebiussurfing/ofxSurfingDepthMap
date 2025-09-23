# ofxSurfingDepthMap - Depth Modes How-To Guide

## Overview
The addon provides 3 depth mapping modes optimized for different scenarios. Each mode processes the 3D depth information differently to create grayscale depth maps suitable for ControlNet processing.

---

## Mode 0: LINEAR (Default)
**What it does:** Direct linear mapping from near to far planes.

**When to use:**
- Simple scenes with uniform depth distribution
- Quick setup without tweaking
- Scenes where objects are evenly distributed in depth

**How to use:**
1. Set `Depth Mode` = 0
2. Adjust `Manual Clips` if needed (camera near/far vs manual values)
3. Fine-tune with `Contrast`, `Brightness`, `Gamma`

**Result:** Close objects = white, far objects = black, linear gradient between.

---

## Mode 1: LOGARITHMIC  
**What it does:** Applies logarithmic curve to depth distribution for better perspective handling.

**When to use:**
- Perspective-heavy scenes (long corridors, landscapes)
- When linear mode produces too much flat gray areas
- Scenes where you need more detail in the mid-distance range

**How to use:**
1. Set `Depth Mode` = 1
2. Compare with Linear mode to see the difference
3. Adjust `Contrast` and `Gamma` to enhance the logarithmic curve
4. Use `Manual Clips` to control the depth range if needed

**Result:** More detail in mid-range depths, smoother transitions, less flat areas.

---

## Mode 2: FOCUS RANGE
**What it does:** Concentrates contrast in a specific depth range, compressing everything else.

**When to use:**
- When you have a specific subject/object to emphasize
- Complex scenes where you want to isolate certain depth layers
- ControlNet applications where specific depth zones are critical

**How to use:**
1. Set `Depth Mode` = 2
2. Click `Auto Focus` button for automatic range calculation (optional)
3. Manually adjust `Focus Near` and `Focus Far` to bracket your subject
4. The `Focus Width` parameter fine-tunes the transition sharpness
5. Objects within focus range get high contrast (mapped to 0.2-0.8 grayscale)
6. Objects outside get compressed to edge values (0.0-0.2 and 0.8-1.0)

**Result:** High contrast in focus zone, compressed contrast outside focus zone.

---

## Global Controls (Work with all modes)

### Camera Ignore
- `Manual Clips` = OFF: Uses camera's near/far clip planes automatically
- `Manual Clips` = ON: Uses your custom `Manual Near` and `Manual Far` values
- **Tip:** Enable manual clips when camera values are unreliable or too extreme

### Tweak Parameters
- `Contrast`: Enhances or reduces depth separation (1.0 = neutral)
- `Brightness`: Shifts entire depth map lighter/darker (0.0 = neutral) 
- `Gamma`: Adjusts curve response (1.0 = linear, <1.0 = brighter mids, >1.0 = darker mids)
- `Invert`: Flips depth map (close=black, far=white)

### Reset Functions
- `Reset Tweaks`: Contrast=1.0, Brightness=0.0, Gamma=1.0, Invert=OFF
- `Reset Mode`: Back to Linear mode, camera clips enabled
- `Reset Manual`: Manual near=0.1, far=2000.0
- `Reset Focus`: Focus range to default values
- `Reset All`: Resets everything to defaults

---

## Workflow Recommendations

### For ControlNet Depth Processing:
1. Start with **Linear mode** for quick results
2. Switch to **Logarithmic** if you see too much flat gray
3. Use **Focus Range** when you need to emphasize specific objects
4. Always check the result with `Invert` toggle to see which works better
5. Save different versions with different modes for comparison

### For Complex Scenes:
1. Use `Manual Clips` to control the depth range precisely  
2. Start with `Auto Focus` in Focus Range mode, then fine-tune manually
3. Adjust `Contrast` and `Gamma` after choosing the right mode
4. Use `Brightness` sparingly - it can clip values

### Troubleshooting:
- **Too much white/black:** Adjust manual near/far clips or use Focus Range mode
- **Flat gray areas:** Try Logarithmic mode or increase Contrast
- **Inverted results:** Toggle the `Invert` parameter
- **Poor subject separation:** Use Focus Range mode with tight focus bounds