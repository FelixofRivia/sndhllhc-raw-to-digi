from pathlib import Path
import subprocess

from libs.processing_status import create_status_table

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

def main():
    directories = {
        "raw" : Path("/home/filippo/conversion"), #Path("/eos/experiment/sndlhc/Run4/testbeam2026/monitoring_test"),
        "converted" : Path("/home/filippo/conversion"), #Path("/eos/experiment/sndlhc/Run4/testbeam2026/converted_data"),
        "histos" : Path("/eos/experiment/sndlhc/www/testbeam2026"),
        "logs" : Path("")
    }

    df = create_status_table(directories)
    output_csv_path = directories["logs"] / "conversion_pipeline_summary.csv"
    df.to_csv(output_csv_path, index=False)

    print(df)
    print(f"\nSaved summary to: {output_csv_path}")

    run_conversion(directories, 100779)

if __name__ == "__main__":
    main()