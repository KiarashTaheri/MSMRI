
---

## 💡 Problem It Solves

Large-scale MRI studies often include scans from different centers, scanners, and protocols. Human verification of the **sequence type (T1, FLAIR, T2, etc.)** is time-consuming and prone to inconsistency.

This tool automates that verification step by:
- Extracting key DICOM metadata
- Classifying protocol type with a machine learning model
- Outputting a standardized sequence label

### 📉 Results:
- 🕐 **~40 hours/month** saved
- 💪 **50%+ reduction** in quality control effort
- 💼 **Integrated into national MS MRI processing pipelines**

---

## 🧠 How It Works

1. **Input**:  
   Basic DICOM metadata:  
   - Echo Time  
   - Repetition Time  
   - Manufacturer  
   - Series Description  
   - Bolus Agent (YES/NO)

2. **Feature Engineering**:  
   - Numeric encoding of times and bolus
   - Bag-of-characters encoding for `Manufacturer` and `Series Description`

3. **Prediction**:  
   - A pretrained `RandomForestClassifier` outputs a letter-based label
   - Label is mapped to the human-readable MRI sequence name (e.g., `T1-weighted`, `FLAIR`, etc.)

---

## 🧪 Example Usage

```bash
python RFclass.py 90 500 Siemens "T1-weighted axial" NO
