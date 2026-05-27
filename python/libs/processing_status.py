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
        converted_file = (directories["converted"] / run_name / f"{run_name}_converted.root")

        dqm_file = directories["histos"] / f"{run_name}_dqm.root"

        # Existence checks
        converted_exists = converted_file.exists()
        dqm_exists = dqm_file.exists()

        # Timestamp comparison
        converted_mtime = file_modified_time(converted_file)
        dqm_mtime = file_modified_time(dqm_file)

        dqm_up_to_date = False

        if converted_exists and dqm_exists:
            dqm_up_to_date = dqm_mtime >= converted_mtime

        rows.append({
            "run": run_number,
            "n_raw": n_raw,
            "n_jsn": n_jsn,
            "converted_modified_time": converted_mtime,
            "dqm_modified_time": dqm_mtime,
            "dqm_up_to_date": dqm_up_to_date,
        })
    
    df = pd.DataFrame(rows)
    if not df.empty:
        df = df.sort_values("run")
    return df

def select_run(df):
    # There cannot be less raw than jsn files
    invalid = df[df["n_raw"] < df["n_jsn"]]

    if not invalid.empty:
        print("ERROR: found rows with n_raw < n_jsn")
        print(invalid[["run", "n_raw", "n_jsn"]])

    # Candidate rows:
    #   - n_raw > n_jsn (needs conversion + dqm)
    #   - n_raw == n_jsn and dqm_up_to_date is False (needs dqm only)
    candidates = df[(df["n_raw"] > df["n_jsn"]) | ((df["n_raw"] == df["n_jsn"]) & (~df["dqm_up_to_date"]))]

    # Select earliest candidate
    selected_run = (
        candidates["run"].min()
        if not candidates.empty
        else None
    )

    return int(selected_run)