from pathlib import Path
import subprocess

def run_dqm(directories, run_number):
    input_root_file = directories['converted'] / f"run{run_number:06d}" / f"run{run_number:06d}_converted.root"
    output_root_file = directories['histos'] / f"run{run_number:06d}_converted.root"
    detinfo_csv = "./../build/tests/data/detector_info.csv"
    make_histos = f"./../build/bin/real_time_monitoring {input_root_file} {detinfo_csv} {output_root_file} 1"
    result = subprocess.run(
        make_histos,
        shell=True,
        executable="/bin/bash",
        capture_output=True,
        text=True
    )
    print(result.stdout)
    print(result.stderr)