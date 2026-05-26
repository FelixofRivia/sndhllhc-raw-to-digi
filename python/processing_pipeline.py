from pathlib import Path

from libs.processing_status import create_status_table

def main():
    directories = {
        "raw" : Path("/eos/experiment/sndlhc/Run4/testbeam2026/monitoring_test"),
        "converted" : Path("/eos/experiment/sndlhc/Run4/testbeam2026/converted_data"),
        "histos" : Path("/eos/experiment/sndlhc/www/testbeam2026")
    }

    OUTPUT_CSV = "run_summary.csv"

    df = create_status_table(directories)
    df.to_csv(OUTPUT_CSV, index=False)

    print(df)
    print(f"\nSaved summary to: {OUTPUT_CSV}")

if __name__ == "__main__":
    main()