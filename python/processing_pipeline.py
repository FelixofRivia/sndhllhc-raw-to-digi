from pathlib import Path
import logging
import sys

from libs.processing_status import select_run, create_status_table
from libs.run_conversion import run_conversion
from libs.run_dqm import run_dqm

def main():
    directories = {
        "raw" : Path("/home/filippo/conversion"), #Path("/eos/experiment/sndlhc/Run4/testbeam2026/monitoring_test"),
        "converted" : Path("/home/filippo/conversion"), #Path("/eos/experiment/sndlhc/Run4/testbeam2026/converted_data"),
        "histos" : Path("/eos/experiment/sndlhc/www/testbeam2026"),
        "logs" : Path(""),
        "cmssw_src" : Path("/home/filippo/CMSSW_15_1_1/src")
    }

    logging.basicConfig(
        level=getattr(logging, "DEBUG"),
        format="%(asctime)s [%(levelname)s] %(message)s",
        datefmt="%H:%M:%S",
        handlers=[
            logging.FileHandler(directories["logs"] / "processing_pipeline_log.txt", mode="a"),  # append mode
            logging.StreamHandler(sys.stdout)          # still print to terminal
        ]
    )

    logging.info("Directories configuration:\n%s", {k: str(v) for k, v in directories.items()})

    df = create_status_table(directories)
    output_csv_path = directories["logs"] / "processing_pipeline_status.csv"
    df.to_csv(output_csv_path, index=False)

    logging.debug("Current pipeline status:\n%s", df.to_string(index=False))
    logging.info("Saved pipeline status to: %s", output_csv_path)

    selected_run = select_run(df)
    if selected_run is not None:
        logging.info("Selected run: %s", selected_run)
        run_conversion(directories, selected_run)
        run_dqm(directories, selected_run)
    else:
        logging.info("Everything is up to date")

if __name__ == "__main__":
    main()