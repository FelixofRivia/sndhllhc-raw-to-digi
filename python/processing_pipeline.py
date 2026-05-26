from pathlib import Path

from libs.processing_status import create_status_table
from libs.run_conversion import run_conversion
from libs.run_dqm import run_dqm

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

    run_conversion(directories, 1000779)
    run_dqm(directories, 1000779)

if __name__ == "__main__":
    main()