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
