# 🚀 Real-Time MRI Artifact Classifier (3D CNN)

This project uses a **3D Convolutional Neural Network** to automatically detect motion artifacts in MRI scans, directly from NIfTI images. By catching artifact-prone scans in **real-time**, the tool prevents patients from needing re-scans, **improves scan throughput**, and **boosts diagnostic reliability**.

> 🧠 Used in preclinical MRI QC pipelines to improve outcomes, reduce patient burden, and increase scanning efficiency.

---

## 🎯 Key Benefits

- ✅ **Real-time detection** of motion artifacts
- ⏱️ **Faster scan-to-diagnosis time**
- 🧍 **Fewer patient re-visits**
- 🏥 **More patients scanned per month**
- 📉 **Reduces bad scan rate in studies and clinics**

---

## 🧠 What It Does

This tool:
1. Loads NIfTI brain MRI scans
2. Resizes them to a standard 3D input shape (e.g., `60x60x20`)
3. Labels whether a scan contains motion artifacts based on a `QC_artifacts` label
4. Trains a **3D CNN** to classify artifact presence
5. Saves the trained model for deployment

---

## 🏗️ Project Files

