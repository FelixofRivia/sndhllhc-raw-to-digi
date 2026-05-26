from pathlib import Path

from libs.processing_status import create_status_table

def main():
    directories = {
        "raw" : Path("/eos/experiment/sndlhc/Run4/testbeam2026/monitoring_test"),
        "converted" : Path("/eos/experiment/sndlhc/Run4/testbeam2026/converted_data"),
        "histos" : Path("/eos/experiment/sndlhc/www/testbeam2026"),
        "logs" : Path("")
    }

    df = create_status_table(directories)
    output_csv_path = directories["logs"] / "conversion_pipeline_summary.csv"
    df.to_csv(output_csv_path, index=False)

    print(df)
    print(f"\nSaved summary to: {output_csv_path}")

if __name__ == "__main__":
    main()