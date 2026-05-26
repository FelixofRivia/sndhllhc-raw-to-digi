from pathlib import Path
import subprocess

def run_conversion(directories, run_number):
    current_dir = Path.cwd()
    source_cms = "source /cvmfs/cms.cern.ch/cmsset_default.sh"
    cmsenv = f"cd /home/filippo/CMSSW_15_1_1/src && cmsenv && cd {current_dir}"
    conversion = f"cmsRun cms_raw_evt_building.py rawDirectory={directories['raw']} convertedDirectory={directories['converted']} runNumber={run_number}"
    result = subprocess.run(
        f"{source_cms} && {cmsenv} && {conversion}",
        shell=True,
        executable="/bin/bash",
        capture_output=True,
        text=True
    )
    print(result.stdout)
    print(result.stderr)