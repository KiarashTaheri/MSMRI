# 🧠 CMIFBet: Brain Extraction & Volume Estimation Tool (C++)

CMIFBet is a command-line C++ tool for brain extraction and volumetric analysis of MRI scans in CMIF format. It interfaces with the **BET (Brain Extraction Tool)** from FSL, converting MRI data into ANALYZE format for processing, and returns a brain-masked version of the original image. This tool was developed for and is actively used by researchers in the **Traboulse Lab** and the **UBC Center for Brain Health**.

---

## 📦 Features

- 🧠 Extracts brain masks from single-channel CMIF MRI files
- 🔄 Converts CMIF to ANALYZE 7.5 format for BET compatibility
- 📁 Writes back the masked brain image into a new CMIF file
- 🧹 Temporary directories and files are handled automatically
- ⚠️ Built with strict safety to avoid overwriting input files

---

## 🔧 Dependencies

- **FSL BET** (Brain Extraction Tool)
- CMIF v5.3+ (custom imaging format used in UBC research pipelines)
- Debian-compatible system (tested on Debian 5.0+)
- No OpenCV required

---

## 🛠️ Build Instructions

### Prerequisites
Ensure the following are available:
- g++ with C++11 or newer
- FSL installed and `BET` accessible in your `PATH`
- Environment variable `MIFBET_INI` pointing to your configuration `.ini`

### Compile

Assuming all dependent `.hpp` and `.cpp` files are in the same folder:

```bash
g++ -o CMIFBet CMIFBet.cpp [other_dependencies].cpp -std=c++11
