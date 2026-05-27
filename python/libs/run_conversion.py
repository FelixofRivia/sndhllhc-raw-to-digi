from pathlib import Path
import subprocess
import logging
import time

def run_conversion(directories, run_number):
    current_dir = Path.cwd()
    source_cms = "source /cvmfs/cms.cern.ch/cmsset_default.sh"
    cmsenv = f"cd {directories['cmssw_src']} && cmsenv && cd {current_dir}"
    clean_previous_conversion = f"rm -f {(directories['raw'] / f'run{run_number:06d}') / '*index*.jsn'}"
    reset_progress = "" #f"echo '1 0' > {(directories['raw'] / f'run{run_number:06d}') / 'fu.lock'}"
    conversion = f"cmsRun cms_raw_evt_building.py rawDirectory={directories['raw']} convertedDirectory={directories['converted']} runNumber={run_number}"

    start = time.perf_counter()

    result = subprocess.run(
        f"{source_cms} && {cmsenv} && {clean_previous_conversion} && {reset_progress} && {conversion}",
        shell=True,
        executable="/bin/bash",
        capture_output=True,
        text=True
    )

    duration = time.perf_counter() - start

    tag = f"[run {run_number:06d}]"

    if result.stdout:
        logging.info("%s Conversion subprocess stdout:\n%s", tag, result.stdout)

    if result.stderr:
        logging.error("%s Conversion subprocess stderr:\n%s", tag, result.stderr)

    logging.info("%s Conversion finished in %.2f seconds", tag, duration)