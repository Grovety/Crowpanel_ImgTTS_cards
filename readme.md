# CrowPanel Image TTS Cards — ESP32-S3 Firmware for Voicing Photo Cards via GRC tinyTTS

**Summary.** Firmware for the ELECROW CrowPanel Advance 5.0 (ESP32-S3, 800×480) with an LVGL UI that displays photo cards and plays the text associated with each card through the GRC tinyTTS module over UART. The app uses a Image screen: you can listen to the card’s story (play) and switch to the next card (change). The card list is looped.

Two **card scenarios** are provided: **album** and **traveling**. The scenario is selected at build time or when flashing prebuilt images (see below).

---

## Repository Structure

- `firmware/` — flashing utility and prebuilt binaries.
  - `binaries_album/` — firmware images with the **album** scenario (ready-made `spiffs.bin` containing “album” cards).  
  - `binaries_travel/` — firmware images with the **traveling** scenario (ready-made `spiffs.bin` containing “traveling” cards).  
- `main/` — firmware sources (ESP-IDF): LVGL UI, UART manager, tinyTTS integration, asset/image subsystem.
- `components/ui/` — SquareLine Studio project and exported LVGL resources. **Scenario configuration** via `scenario_build.h`.
- `spiffs_root/` — asset sources:
  - `spiffs_root/assets_alb/` — RAW frames (binary image representations) and descriptive texts for the **album** scenario;
  - `spiffs_root/assets_trav/` — RAW frames and descriptive texts for the **traveling** scenario.  
  Assets are packed into SPIFFS (`spiffs.bin`) at build time.

---

## Features

- **SPIFFS → PSRAM → LVGL pipeline:** RAW frames are stored in the SPIFFS flash FS. Before display, a frame is read **entirely** into a **PSRAM** buffer and rendered via the LVGL/driver stack.
- **Image ↔ text binding:** each card has a descriptive text for speech playback.
- **Embedded device focus:** Designed for ELECROW CrowPanel Advance 5.0-HMI. For detailed device hardware information, see [Device Hardware Documentation](https://www.elecrow.com/pub/wiki/CrowPanel_Advance_5.0-HMI_ESP32_AI_Display.html).  
- **tinyTTS control:** Load text into the buffer, trigger playback, monitor playback status, and adjust volume using the GRC tinyTTS module. For details, see [HxTTS repository](https://github.com/Grovety/HxTTS).  
- **Persistent settings:** Saved to NVS / file for convenient reuse.  

---

## Scenario Selection (album vs traveling)

### Option A — **at build time** from sources
The file `components/ui/scenario_build.h` must define **exactly one** scenario:
```c
// components/ui/scenario_build.h
// Set exactly one flag to 1, the other to 0
#define UI_SCENARIO_ALBUM   1
#define UI_SCENARIO_TRAVEL  0
```
> If both or neither are defined, the build fails.  
> The build system generates `spiffs.bin` with the appropriate card set from `spiffs_root/assets_<scenario>/`.

**Changing the scenario?**  
Because scenario selection affects generated assets and build artifacts, run a **full clean** before rebuilding:

```bash
idf.py fullclean
idf.py build
```

### Option B — **when flashing prebuilt images**
Two image sets are available under `firmware/`:
- `binaries_album/` — firmware with the **album** scenario;
- `binaries_travel/` — firmware with the **traveling** scenario.

When flashing, point the flasher to the corresponding folder (see “Flashing” → “Prebuilt images”).

---

# Usage 

> **IMPORTANT:** Order of actions matters — first connect the CrowPanel to your computer via USB/Serial, then flash the board, then connect the GRC tinyTTS module.

1. **Hardware setup (initial)**  
   - Set the function-select switch on the CrowPanel to **WM**(0,1) (UART1-OUT mode).  
   - Connect the CrowPanel to the PC via USB/Serial.  
   - **Do NOT connect the GRC tinyTTS module yet.** The tinyTTS module must be connected only after the panel firmware has been flashed (see note below).  

2. **Flashing the board**

#### Build from sources
1. Install ESP-IDF framework v5.4.
2. ```git lfs install```  
3. Clone the repository.
4. Select a scenario in `components/ui/scenario_build.h`.
5. Dependencies are managed via the ESP-IDF component manager (`idf_component.yml`).  
6. Build and flash:
```bash
idf.py build
idf.py -p PORT flash
```

 ### Flashing Prebuilt Images
Use the strict flasher in `firmware/` (see `firmware/flash_tool.md`).

Required files inside the selected folder:
- `bootloader.bin` @ `0x0000`
- `partition-table.bin` @ `0x8000`
- `proj.bin` @ `0x10000`
- `spiffs.bin` @ `0x110000`

3. **Hardware setup (post-flash)**  
   - After flashing completes and the device has rebooted, connect the GRC tinyTTS module (UART0) and CrowPanel (UART1-OUT) using a 4-pin adapter.  
   - Connect the audio output (3.5 mm mini jack) from the tinyTTS module to the speaker/headphones.

4. **Screen Logic & Controls**  

- `ui_img` — the current card image from SPIFFS.
- `play` — speak the card’s text via tinyTTS.
- `change` — go to the next card (looped list).
- Service statuses/errors — visible in the serial log.

---

# Asset Subsystem (SPIFFS → PSRAM → LVGL)

- **Storage:** RAW frames reside in SPIFFS.
- **Rendering:** a frame is read **entirely** into a PSRAM buffer, then displayed via LVGL/driver.

---

# UI (SquareLine Studio / LVGL)

- UI sources: `components/ui/` (SquareLine project + generated LVGL files).
- **Image screen:**
  - `ui_img` widget — displays the current photo card from SPIFFS;
  - **`play`** button — sends the card’s descriptive text to tinyTTS and starts playback;
  - **`change`** button (arrow) — shows the **next** card. After the last card the list wraps to the first (looped).
![](images/ui_image.png)

---

## Dependencies

- **ESP-IDF Components:** all dependencies are declared in `idf_component.yml` and fetched automatically by the ESP-IDF component manager.
- **LVGL 8.4:** UI rendering (exported from SquareLine Studio).
- **GRC tinyTTS integration:** Serial/UART transport and the module’s register-level protocol.
