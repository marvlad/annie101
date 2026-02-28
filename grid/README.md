# ANNIE PMT Data (grid)

## Classes used:
This is a simple test to run in the grid. The main python script is 
- `grid-temp`

It will produce the following scripts: `submit_job.sh, grid_instructions.sh, and instructions_into_container.sh`. For this case we won't use `instructions_into_container.sh`

## Step 1 
`python3 grid-temp`

## Step 2
Modify in the `submit_job.sh` the INPUT_PATH and OUTPUT_PATH. Similarly in the change grid_instructions.sh  the GRIDUSER

## Step 3
Compile the code in `annie101_A3` (one way is to access to the container and source Setup.sh and make)

## Step 4
Compress annie101_A3 (`tar -cvzf annie101_A3.tar.gz annie101_A3`) and copy to INPUT_PATH

## Step 5
Run `submit_job.sh` like `./submit_job.sh RUN_NUMBER PARTIFILE`

