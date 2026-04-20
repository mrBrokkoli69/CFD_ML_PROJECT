#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="/run/media/deck/SD512/6sem/IT_PROJECT/CFD_ML_PROJECT"
SRC_FILE="$PROJECT_DIR/src/main_lbm.cpp"
RUN_DIR="$PROJECT_DIR/data/case_runs"
EXECUTABLE="$PROJECT_DIR/lbm_test"

mkdir -p "$RUN_DIR"

BACKUP_FILE="$(mktemp)"
cp "$SRC_FILE" "$BACKUP_FILE"

restore_original() {
    if [[ -f "$BACKUP_FILE" ]]; then
        cp "$BACKUP_FILE" "$SRC_FILE"
        rm -f "$BACKUP_FILE"
    fi
}
trap restore_original EXIT

SUMMARY_FILE="$RUN_DIR/summary.csv"
echo "case_name,nx,ny,tau,u_in,r,maxSteps,vtkInterval,Re,Fx,Fy,Cd" > "$SUMMARY_FILE"

replace_line() {
    local pattern="$1"
    local replacement="$2"
    local file="$3"

    if ! grep -Eq "$pattern" "$file"; then
        echo "ERROR: pattern not found: $pattern"
        exit 1
    fi

    sed -E -i "s|$pattern|$replacement|" "$file"
}

run_case() {
    local case_name="$1"
    local nx="$2"
    local ny="$3"
    local tau="$4"
    local u_in="$5"
    local r="$6"
    local maxSteps="$7"
    local vtkInterval="$8"

    local case_dir="$RUN_DIR/$case_name"
    mkdir -p "$case_dir"

    echo "=== Running $case_name ==="

    cp "$BACKUP_FILE" "$SRC_FILE"

    replace_line '^[[:space:]]*const int nx[[:space:]]*=.*;$' \
                 "    const int nx = $nx;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*const int ny[[:space:]]*=.*;$' \
                 "    const int ny = $ny;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*const double tau[[:space:]]*=.*;$' \
                 "    const double tau = $tau;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*const double u_in[[:space:]]*=.*;$' \
                 "    const double u_in = $u_in;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*int cx[[:space:]]*=.*;$' \
                 "    int cx = nx / 3;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*int cy[[:space:]]*=.*;$' \
                 "    int cy = ny / 2;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*int r[[:space:]]*=.*;$' \
                 "    int r = $r;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*const int maxSteps[[:space:]]*=.*;$' \
                 "    const int maxSteps = $maxSteps;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*const int vtkInterval[[:space:]]*=.*;$' \
                 "    const int vtkInterval = $vtkInterval;" \
                 "$SRC_FILE"

    replace_line '^[[:space:]]*const std::string outputDir[[:space:]]*=.*;$' \
                 "    const std::string outputDir = \"$case_dir\";" \
                 "$SRC_FILE"

    grep -n "outputDir" "$SRC_FILE" > "$case_dir/output_dir_check.log"

    (
        cd "$PROJECT_DIR"
        make > "$case_dir/build.log" 2>&1
    )

    (
        cd "$PROJECT_DIR"
        ./lbm_test > "$case_dir/run.log" 2>&1
    )

    cp "$SRC_FILE" "$case_dir/main_lbm_used.cpp"

    local Re=""
    local Fx=""
    local Fy=""
    local Cd=""

    if grep -q "^Re =" "$case_dir/run.log"; then
        Re="$(grep "^Re =" "$case_dir/run.log" | tail -n 1 | sed 's/^Re = //')"
    fi

    if grep -q "Final force:" "$case_dir/run.log"; then
        Fx="$(grep "Final force:" "$case_dir/run.log" | tail -n 1 | sed -E 's/.*Fx = ([^,]+), Fy = .*/\1/')"
        Fy="$(grep "Final force:" "$case_dir/run.log" | tail -n 1 | sed -E 's/.*Fy = ([^ ]+).*/\1/')"
    fi

    if grep -q "^Cd =" "$case_dir/run.log"; then
        Cd="$(grep "^Cd =" "$case_dir/run.log" | tail -n 1 | sed 's/^Cd = //')"
    fi

    echo "$case_name,$nx,$ny,$tau,$u_in,$r,$maxSteps,$vtkInterval,$Re,$Fx,$Fy,$Cd" >> "$SUMMARY_FILE"
}

run_case "case1_lowRe_sanity"       256 100 0.65  0.05  5  2500 10
run_case "case2_benchmark_like"     256 100 0.65 0.075 10  4000 10
run_case "case3_sameRe_coarse"      128  50 0.65 0.075  5  2500 10
run_case "case4_sameRe_fine"        512 200 0.65 0.075 20  6000 10
run_case "case5_higherRe_steadyish" 256 100 0.65  0.10 10  5000 10
run_case "case6_stress_test"        256 100 0.58  0.10 10  6000 10
run_case "case7_old_repro_case"     256 100 0.75  0.10  5  5000 10

echo
echo "All cases finished."
echo "Summary saved to: $SUMMARY_FILE"
