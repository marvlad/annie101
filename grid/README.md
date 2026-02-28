# ANNIE PMT Data Processing (Grid)

This repository provides a lightweight workflow to process ANNIE PMT data on the grid using an Apptainer container environment.

---

## Overview

The main script is:

`grid-temp`

Running this script generates the required grid submission files:

- `submit_job.sh`
- `grid_instructions.sh`
- `instructions_into_container.sh`

> **Note:** In this workflow, `instructions_into_container.sh` is not used.

---

## Workflow

### Step 1 — Generate Grid Scripts

Run:

```bash
python3 grid-temp
```
### Step 2 — Configure Grid Parameters

Before submitting jobs, edit the following files:

submit_job.sh
- Modify:
- INPUT_PATH
- OUTPUT_PATH

grid_instructions.sh
Modify:

- GRIDUSER

Ensure that the paths and user configuration match your grid environment.

### Step 3 — Compile the Code

Compile the code inside the annie101_A3 directory.

One possible approach:
```
apptainer exec <container_image> bash -lc "source Setup.sh && make"
```
Alternatively:

- Enter the container manually.
- Source Setup.sh.
- Run make.

### Step 4 — Package the Code
Compress the compiled directory:
```
tar -cvzf annie101_A3.tar.gz annie101_A3
```
Then copy the tarball to your configured `INPUT_PATH`.

### Step 5 — Submit the Job
Submit a grid job using:
```
./submit_job.sh RUN_NUMBER PARTIFILE
```
Where:

- RUN_NUMBER — Run identifier (e.g., 5090)
- PARTIFILE — Part file number (e.g., 217)

Example:
```
./submit_job.sh 5090 217
```
