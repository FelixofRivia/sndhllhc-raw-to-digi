from pathlib import Path

from libs.processing_status import select_run, create_status_table
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

    selected_run = select_run(df)
    if selected_run:
        print("selected_run =", selected_run)
        run_conversion(directories, selected_run)
        run_dqm(directories, selected_run)
    else:
        print("Everything is up to date")

if __name__ == "__main__":
    main()