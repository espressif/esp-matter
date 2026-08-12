#! /bin/bash

ESP_MATTER_PATH=$PWD

# data model generator and the unit tests
# these do not work with Python3.14 so pinning to 3.12 for now
python3.12 -m venv /tmp/data_model_gen_venv
source /tmp/data_model_gen_venv/bin/activate

pip install -r tools/data_model_gen/requirements.txt
pip install pre-commit
pre-commit install-hooks

cd tools/data_model_gen
ESP_MATTER_PATH=$ESP_MATTER_PATH python3 data_model_gen.py --clean --no-colored-logs
python3 tests/run_tests.py -v
cd -

find components/esp_matter/data_model/generated -name '*.cpp' -o -name '*.h' | xargs pre-commit run --files

deactivate

python3 components/esp_matter/zap_common/generate_zap_common_files.py
python3 components/esp_matter/utils/cluster_select/generate_cluster_select_files.py
