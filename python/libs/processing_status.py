from datetime import datetime
import pandas as pd

def file_modified_time(path):
    if path.exists():
        return datetime.fromtimestamp(path.stat().st_mtime)
    return None
    

def create_status_table(directories):
    rows = []
    for run_dir in sorted(directories["raw"].glob("run*")):

        if not run_dir.is_dir():
            continue

        run_name = run_dir.name
        run_number = run_name.replace("run", "")

        # Count files
        raw_files = list(run_dir.glob("*index*.raw"))
        jsn_files = list(run_dir.glob("*index*.jsn"))

        n_raw = len(raw_files)
        n_jsn = len(jsn_files)

        # Corresponding files
        converted_file = (
            directories["converted"] / run_name / f"{run_name}_converted.root"
        )

        qdm_file = directories["histos"] / f"{run_name}_qdm.root"

        # Existence checks
        converted_exists = converted_file.exists()
        qdm_exists = qdm_file.exists()

        # Timestamp comparison
        converted_mtime = file_modified_time(converted_file)
        qdm_mtime = file_modified_time(qdm_file)

        qdm_up_to_date = False

        if converted_exists and qdm_exists:
            qdm_up_to_date = qdm_mtime >= converted_mtime

        rows.append({
            "run": run_number,
            "n_raw": n_raw,
            "n_jsn": n_jsn,
            "converted_modified_time": converted_mtime,
            "qdm_modified_time": qdm_mtime,
            "qdm_up_to_date": qdm_up_to_date,
        })
    
    df = pd.DataFrame(rows)
    if not df.empty:
        df = df.sort_values("run")
    return df