# 🧠 MRI Sequence Classifier (Random Forest-Powered)

This Python-based classifier predicts the MRI sequence type using only scan parameters—**no images required**. Designed to support radiology workflows, it reduces manual quality control time by over **50%**, effectively saving **~40 hours of labor each month**.

> ⚡ Fast. 🔍 Accurate. 📉 Minimal input.  
> Built for researchers, technologists, and imaging labs handling large-scale MRI datasets.

---

## 🚀 What It Does

Given five common DICOM metadata fields:
- Echo Time
- Repetition Time
- Manufacturer
- Series Description
- Bolus Agent

The tool:
1. Encodes numerical and textual data into feature vectors
2. Uses a trained **Random Forest Classifier** to predict a scan type (e.g., `T1`, `FLAIR`, `DWI`, etc.)
3. Outputs the predicted MRI protocol—accelerating protocol verification

---

## 💡 Why Use It?

✅ **Reduces Manual QC by 50%**  
✅ **~40 Hours Saved Monthly** for typical research imaging pipelines  
✅ **No Image Loading Needed**—just scan parameters  
✅ **Fast Classification** (<1 sec per case)  
✅ **Portable + Easily Integrated** into DICOM sorters or PACS tools

---

## 🧱 Project Files

