# 📅 Exam Planning System

This project is a **C language-based application** for automating the **planning of university exams**, developed as part of a mini-project at ENSA Al Hoceima.

## ⚠️ Prerequisite - Important Notice

> **In order for the application to work properly, you must install the Haru Free PDF Library (libharu).**

- Visit the official repository: [https://github.com/libharu/libharu](https://github.com/libharu/libharu)
- Follow the installation instructions specific to your operating system.
- This library is used to generate PDF versions of the exam schedules.

## 👨‍🏫 Project Overview

The main goal of this system is to address the lack of a simple and efficient tool for generating exam schedules at the university. It provides:

- Automatic generation of exam schedules based on data such as professors, modules, rooms, etc.
- A dedicated **Admin space** for creating and managing schedules.
- A **Student space** where students can register and access their personal exam timetables.

## 🧱 System Architecture

### Data Models

- **Conceptual Model (MCD)**: Entities include `Professor`, `Module`, `Student`, and `Room`, with their logical associations.
- **Logical Model (MLD)**: Translates into four main tables:
  - `Module (ID, Name, Field, Semester, ProfessorID)`
  - `Professor (ID, Name)`
  - `Student (Name, Surname, Email, Password)`
  - `Room (RoomName)`

## 🔧 Technical Details

The application is built using standard and external C libraries:

- `hpdf.h`: Generates PDF files for exam schedules.
- `stdio.h`, `stdlib.h`, `string.h`: Standard C libraries for input/output, memory, and string operations.
- `time.h`: For managing and formatting exam dates.
- `windows.h`, `conio.h`, `ctype.h`: Windows-specific and console handling libraries.

## 🖥️ Interfaces

### 🔐 Admin Space

- **Login required**: Default credentials are `Adm / Adm1456`.
- **Features**:
  - Generate schedules by **session** (e.g., Autumn, Spring) for all departments.
  - Generate schedules by **field and semester** (e.g., TDIA S1).
- **Output**: PDF schedules saved under `Planning/<Field>/`.

### 🎓 Student Space

- **Options**:
  - **Register**: Name, surname, and password stored in `Data/users.txt`.
  - **Login**: Requires institutional email (`nom.prénom@etu.uae.ac.ma`) and password.
- **Access**: View personalized exam schedule in PDF format based on field and semester.

## 🧑‍💻 Authors

- [HAMIMID Khalil](#)
- [BEN TALEB Fatima Zahrae](#)
- [EL FADEL Soufiane](#)

## 🎓 Supervised by

**Dr. Bahri Abdelkhalek**

---

> 🏫 *Developed at ENSA Al Hoceima as part of the C programming curriculum.*
