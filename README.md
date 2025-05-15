# 🧠 MRI Intelligence Suite  
### End-to-End Quality Control & Sequence Classification for National MRI Pipelines

The **MRI Intelligence Suite** is a complete, production-grade platform that automates MRI sequence labeling, motion artifact detection, brain extraction, and national-scale image uploads. It’s currently integrated into the **Multiple Sclerosis (MS) MRI pipeline** across **research centers in Canada**, supporting thousands of MRI scans annually.

---

## 🚀 Why This Matters

MRI quality control is time-intensive, error-prone, and inconsistent across institutions. This suite automates that entire process—ensuring:
- ✅ More **accurate data**
- ✅ **Fewer patient re-scans**
- ✅ **Faster throughput** per scanner
- ✅ **Standardization across sites**

> 🔬 **Used in production pipelines** at leading research hospitals  
> 🕓 **~40 hours/month** of manual QC eliminated  
> 🎯 **98% classification accuracy** using metadata  
> 🧠 **92% artifact detection accuracy** using 3D convolutional deep learning

---

## 💡 Key Components

### 📘 MRI Sequence Classifier (Random Forest – 98% Accuracy)

Classifies MRI scans like `T1`, `FLAIR`, `T2`, and `DWI` using only metadata:
- Inputs: Echo Time, Repetition Time, Manufacturer, Series Description, Bolus Agent
- Combines numerical and character-level features
- Trained on thousands of labeled examples from Canadian MS studies

> **Impact**:  
> Saves technologists hours of manual label review  
> Enables harmonization of sequence types across scanners and vendors

---

### ⚡ Motion Artifact Detector (3D CNN – 92% Accuracy)

Detects motion artifacts directly from raw `.nii` brain MRI volumes using a 3D convolutional neural network:
- Volumes are resized and normalized to a fixed input shape
- Model is trained using real-world QC labels (`artifact` vs. `clean`)
- Inference time < 1 second per scan

> **Impact**:  
> Flags motion artifacts in **real-time**  
> Prevents missed QC errors and unnecessary repeat scans  
> **Improves patient outcomes** and **scanner throughput**

---

### 🧠 Brain Extraction Tool (C++ + FSL BET)

A high-performance C++ wrapper for FSL’s BET tool:
- Converts CMIF MRI volumes into ANALYZE format
- Performs skull stripping using BET
- Converts and writes back the masked brain into CMIF
- Manages temporary files and directory cleanup

> **Use Case**: Automates preprocessing in neuroimaging pipelines (e.g., lesion segmentation, registration)

---

### 🌐 Web-Based Upload Portal

A secure and user-friendly interface for researchers to upload MRI or blood scan data:
- Used across Canadian research centers
- Stores data in a centralized MySQL backend
- Handles high-volume uploads with resilience and consistency

> **Impact**:  
> Enables rapid data aggregation from hospitals and institutions  
> Supports multi-site collaboration and downstream machine learning tasks

---

## 🎯 Real-World Results

| Metric                          | Value                          |
|----------------------------------|--------------------------------|
| **Quality Control Time Reduced** | ✅ ~50%                        |
| **Monthly Hours Saved**         | ⏱ ~40 hours                   |
| **Sequence Classification**     | 🎯 98% accuracy (RF)           |
| **Artifact Detection**          | 🧠 92% accuracy (3D CNN)        |
| **Website Upload Portal**       | 🌐 Enables large-scale researcher uploads across Canada |
| **Active Deployment**           | 🌐 Canada-wide MRI pipelines    |
| **Patient Impact**              | 🧍 Fewer re-scans, faster care |

---

## 🛠 Technical Highlights

- Built using **Python, TensorFlow, scikit-learn, Keras, C++, PHP, MySQL**
- Modular, scalable, and adaptable for clinical or research integration
- Tested with data from **UBC Center for Brain Health**, **Traboulse Lab**, and **multi-site MS studies**

---

## 👨‍🔬 Author

**Kiarash Taheri**  
Biomedical Engineering @ UBC  
AI in Healthcare · Neuroimaging · Signal Processing

---

## 📄 License

This project is intended for **research and academic use only**.  
Contact the author for clinical deployment or commercial licensing opportunities.

---

## 🙏 Acknowledgments

Special thanks to:
- **UBC Center for Brain Health**  
- **Traboulse Lab**, UBC Faculty of Medicine  
- Technologists, radiologists, and data contributors across **Canada**
