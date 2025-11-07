# Adding a Custom Scenario — Example: **third**

## 1) Project Structure Cheat Sheet

```
components/
  ui/
    include/
      scenario_build.h         (←) select the active scenario (exactly one = 1)
      builtin_texts.h
      visuals.h
    third/                     (←) NEW: visuals code and optional C image arrays
      visuals_third.c          
      img_third_01.c
      img_third_02.c
      img_third_03.c
    builtin_texts_third.c      (←) NEW: scenario texts
    CMakeLists.txt             (←) add new block for scenario
spiffs_root/
  assets_third/                (←) NEW: binary resources (*.bin) for the third scenario
    ui_img_third_01_png.bin
    ui_img_third_02_png.bin
    ui_img_third_03_png.bin
```

---

## 2) Enable the Scenario in `scenario_build.h`  

**File:** `components/ui/include/scenario_build.h`

Set **exactly one** flag to `1` (leave all others `0`). For the **third** scenario:

```c
// Set exactly ONE to 1, others to 0
#define UI_SCENARIO_ALBUM    0
#define UI_SCENARIO_TRAVEL   0
#define UI_SCENARIO_THIRD    1   // ← activate the new scenario
```

---

## 3) Create Scenario Assets Folder  

**Folder:** `spiffs_root/assets_third/`

Place **.bin** images here. These are the RAW binaries loaded from SPIFFS at runtime.

Example:
```
spiffs_root/
  assets_third/
    ui_img_third_01_png.bin
    ui_img_third_02_png.bin
    ui_img_third_03_png.bin
```

---

## 4) C Image Arrays (`img_*.c`) 

After generate **C arrays** for images (`img_*.c`), store them **in the scenario folder**:

**Path:** `components\ui\third`

Example:
```
components/ui/third/
  visuals_third.c
  ui_img_third_01_png.c
  ui_img_third_02_png.c
  ui_img_third_03_png.c
```

**Important:** the universal `components/ui/CMakeLists.txt` collects all `*.c` from `components/ui/<scenario-name-lower>/`, i.e. from `components/ui/third/`.  
The mapping is strict: if `UI_SCENARIO_THIRD` is enabled in `components/ui/include/scenario_build.h`, the folder **must** be named `third`. Otherwise it will not be picked up (CMake resolves the directory via `lowercase(<macro-name>)`).

---

## 5) Produce RAW Binary Images

### Option A — SquareLine Studio (SLS)

1. Open the project in **SquareLine Studio** (LVGL v8).  
2. Create/Configure export:
   - set the **export path**;
   - set output format to **binary raw**.
3. Import images: *Assets* → **Import** (PNG / UI frame sample size 578×339).  
4. Export/Generate: **Export UI files**.
   - Copy generated `*.bin` from `export_root/drive/assets` to `spiffs_root/assets_third/`.
   - Copy generated C arrays `img_*.c` from your “UI file export path” to `components/ui/third/`.

### Option B — LVGL Image Converter (official online tool)

1. Open **LVGL Image Converter**.  
2. Upload PNG/JPG.  
3. Parameters:
   - **Color format:** `True color (RGB565)`;
   - **Alpha:** `None` (or as required);
   - **Output:** `Binary`.  
4. Download `*.bin` and place them in `spiffs_root/assets_third/`.  
   For C arrays, select **C array** and save resulting `img_*.c` files into `components/ui/third/`.

---

## 6) Add Scenario Texts 

**File:** `components/ui/builtin_texts_third.c`

Minimal 3-text scaffold (you may define more or fewer; `builtin_text_count()` is supported):

```c
#include "builtin_texts.h"
#include <stdatomic.h>

static const char* kThirdTexts[] = {
    // CASE_TXT_01
    "Row 1.\n"
    "Row 2.\n"
    "Row 3.",

    // CASE_TXT_02
    "Row 1.\n"
    "Row 2.\n"
    "Row 3.",

    // CASE_TXT_03
    "Row 1.\n"
    "Row 2.\n"
    "Row 3."
};

static _Atomic(builtin_text_case_t) s_current_case = CASE_TXT_01;

static inline int clamp_index(int idx, int n) {
    if (n <= 0) return 0;
    if (idx < 0) return 0;
    if (idx >= n) return n - 1;
    return idx;
}

int builtin_text_count(void) {
    return (int)(sizeof(kThirdTexts) / sizeof(kThirdTexts[0]));
}

...
```

---

## 7) Add Scenario Visuals (Images) 

**Folder:** `components/ui/third/`  
**File:** `components/ui/third/visuals_third.c`

```c
// visuals_third.c — case-to-image mapping for scenario "third" (←)
#include "visuals.h"

/* Declarations from your img_third_*.c (if you use C arrays) */
extern const lv_img_dsc_t ui_img_third_01; void ui_img_third_01_load(void);
extern const lv_img_dsc_t ui_img_third_02; void ui_img_third_02_load(void);
extern const lv_img_dsc_t ui_img_third_03; void ui_img_third_03_load(void);

const case_visual_t kVisuals[CASE_TXT_COUNT] = {
    [CASE_TXT_01] = { &ui_img_third_01, ui_img_third_01_load },
    [CASE_TXT_02] = { &ui_img_third_02, ui_img_third_02_load },
    [CASE_TXT_03] = { &ui_img_third_03, ui_img_third_03_load },
    /* Remaining entries may stay zero-initialized */
};
```

---
## 8) Block in components/ui/CMakeLists.txt

If your files and assets are already in place, it’s enough to **uncomment/change** this CMake fragment inside `components/ui/CMakeLists.txt `

```
# -----------------------------------------------------------------------------
# ADD A CUSTOM SCENARIO ("third"):
# -----------------------------------------------------------------------------
elseif(_SCEN_SELECTED_NAME_LOWER STREQUAL "third")
    list(APPEND UI_SCENARIO_SRCS
        ${CMAKE_CURRENT_LIST_DIR}/builtin_texts_third.c
        ${CMAKE_CURRENT_LIST_DIR}/third/visuals_third.c
    )
```

---

## 9) Build & Verify

1. Run:
   ```
   idf.py fullclean
   idf.py build
   idf.py -p PORT flash monitor
   ```
2. During configuration you should see the selected scenario in logs:
   ```
   -- UI scenario selected: THIRD (lower='third')
   ```
