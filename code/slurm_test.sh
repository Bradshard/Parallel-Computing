#!/bin/bash
# This file test.sh is a sample script to run the many jobs by making use of multiple cores 
# To have output file defined as slurm-%A_%a, run your script with sbatch -o slurm-%A_%a.out test.sh 
# command where %A is the id of the submitted jobs and %a is \{0..ntasks\} 
# To have one output file for all simultaneous task, run your script with sbatch test.sh \
# set the partition where the job will run \
#SBATCH --partition=577Q 
## The following line defines the name of the submitted job \
#SBATCH --job-name=slurm_test 
#
## The default output file if we run the script with the command sbatch test.sh \
#SBATCH --output=slurm_output-%j.txt 
#
## set the number of nodes and processes per node \
## That is, we will run this many tasks simultaneously \
#SBATCH --nodes=1
##SBATCH --ntasks=2
#
## mail alert at start, end and abortion of execution \
## The user will be mailed when the job starts and stops or aborts \
## --mail-type=<type> where <type> may be BEGIN, END, FAIL, REQUEUE or ALL (for any change of job state \
#SBATCH --mail-type=BEGIN,END
#
## send mail to this address
#SBATCH --mail-user=<user_name>@ceng.metu.edu.tr

# Launch the command/application 
mpirun cyclic_parallel "$1"

