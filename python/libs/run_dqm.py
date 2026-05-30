import subprocess
import logging
import time

def run_dqm(directories, run_number):
    tag = f"[run {run_number:06d}]"

    input_root_file = directories['converted'] / f"run{run_number:06d}" / f"run{run_number:06d}_converted.root"
    output_root_file = directories['histos'] / f"run{run_number:06d}_dqm.root"
    detinfo_csv = "./../tests/data/detector_info_tb2026.csv"

    command = f"./../build/bin/real_time_monitoring {input_root_file} {detinfo_csv} {directories["geometry"]} {output_root_file} 1"

    logging.debug("%s Running DQM command: %s", tag, command)

    start = time.perf_counter()
    
    result = subprocess.run(
        command,
        shell=True,
        executable="/bin/bash",
        capture_output=True,
        text=True
    )

    duration = time.perf_counter() - start

    if result.stdout:
        logging.info("%s DQM subprocess stdout:\n%s", tag, result.stdout)

    if result.stderr:
        logging.error("%s DQM subprocess stderr:\n%s", tag, result.stderr)

    logging.info("%s DQM finished in %.2f seconds", tag, duration)