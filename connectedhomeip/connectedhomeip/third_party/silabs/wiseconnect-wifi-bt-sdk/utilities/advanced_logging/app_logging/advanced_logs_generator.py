import os
import time
import json

current_path = os.getcwd()
json_path = current_path
search_path = current_path[:-len("\\utilities\\advanced_logging\\app_logging"):] + "/sapi"
autogen_path = search_path + "/include"
preferred_database = {}
database = {}
combined_database = {}
final_database = {}

# Error check
def error_check(line, code_line, database, numarg, filename, c_line):
    # minimum number of arguments check
    if '"' in line:
        count = 0
        for elmnt in line:
            if elmnt == '"':
                break
            if elmnt == ",":
                count += 1
        if count < 3:
            raise RuntimeError(
                "Error: Mandatory arguments missing in "
                + c_line
                + " in file - "
                + filename
            )
    elif len(code_line) < 3:
        raise RuntimeError(
            "Error: Mandatory arguments missing in " + c_line + " in file - " + filename
        )

    # argument number check
    argnum = 0
    no_arg = True
    for elmnt in line[::-1]:
        if elmnt == '"':
            no_arg = False
            break
        elif elmnt == ",":
            argnum += 1
    if numarg != argnum and no_arg == False:
        raise RuntimeError(
            "Error: Argument Count Mismatch for " + c_line + " in file - " + filename
        )

    # Debug_id is already present and format mismatch check
    if code_line[0] in database:
        if numarg != database[code_line[0]]["argnum"]:
            raise RuntimeError(
                "Error: Argument Count Mismatch between existing DebugID - "
                + c_line
                + " in file - "
                + filename
                + " "
                + str(database[code_line[0]])
            )
        current_list = get_format(line, filename, c_line)
        for num in range(numarg):
            if (
                database[code_line[0]]["arg" + str(num + 1) + "type"]
                != current_list[num]
            ):
                raise RuntimeError(
                    "Error: Argument Type Mismatch with existing DebugID for "
                    + c_line
                    + " in file - "
                    + filename
                )


# creating a database
# no preference dictionary
def add_db(line, code_line, database, filename, c_line):
    database[code_line[0]] = {
        "argnum": numarg,
        "component": code_line[1],
        "level": code_line[2],
    }
    if numarg != 0:
        database[code_line[0]]["format"] = line.split('"')[1]
    else:
        database[code_line[0]]["format"] = ""
    format_list = get_format(line, filename, c_line)
    for num in range(numarg):
        database[code_line[0]]["arg" + str(num + 1) + "type"] = format_list[num]


# preference dictionary
def add_pref_db(line, code_line, preferred_database, filename, c_line):
    preferred_database[code_line[0]] = {
        "argnum": numarg,
        "component": code_line[1],
        "level": code_line[2],
    }
    if numarg != 0:
        preferred_database[code_line[0]]["format"] = line.split('"')[1]
    else:
        preferred_database[code_line[0]]["format"] = ""
    format_list = get_format(line, filename, c_line)
    for num in range(numarg):
        preferred_database[code_line[0]]["arg" + str(num + 1) + "type"] = format_list[
            num
        ]


# obtain number of args
def get_numargs(line):
    argnum = 0
    for val in line:
        if val == "%":
            argnum += 1
    return argnum


# create sl_printf_debug_id format
def get_new_slprintf(debug_id, combined_database):
    if combined_database[debug_id]["argnum"] == 0:
        new_line = "SL_PRINTF_" + debug_id + "(debug_id, component, level)"
    elif combined_database[debug_id]["argnum"] == 1:
        new_line = "SL_PRINTF_" + debug_id + "(debug_id, component, level, format, a)"
    elif combined_database[debug_id]["argnum"] == 2:
        new_line = "SL_PRINTF_" + debug_id + "(debug_id, component, level, format, a, b)"
    elif combined_database[debug_id]["argnum"] == 3:
        new_line = "SL_PRINTF_" + debug_id + "(debug_id, component, level, format, a, b, c)"
    return new_line


# create sl_log format
def get_sl_log(debug_id, combined_database):
    if combined_database[debug_id]["argnum"] == 0:
        sl_log_line = "sl_log_" + "no" + "_args(" + debug_id + "_SIGNATURE(component, level)) "
    elif combined_database[debug_id]["argnum"] == 1:
        sl_log_line = (
            "sl_log_"
            + str(combined_database[debug_id]["argnum"])
            + "_args("
            + debug_id
            + "_SIGNATURE(component, level), a)"
        )
    elif combined_database[debug_id]["argnum"] == 2:
        sl_log_line = (
            "sl_log_"
            + str(combined_database[debug_id]["argnum"])
            + "_args("
            + debug_id
            + "_SIGNATURE(component, level), a, b)"
        )
    elif combined_database[debug_id]["argnum"] == 3:
        sl_log_line = (
            "sl_log_"
            + str(combined_database[debug_id]["argnum"])
            + "_args("
            + debug_id
            + "_SIGNATURE(component, level), a, b, c)"
        )
    return sl_log_line


# obtain combined database
def merge_db(dict1, dict2):
    res = {**dict1, **dict2}
    return res


# obtain format type
def get_format(line, filename, c_line):
    format_list = []
    for val in range(len(line)):
        if line[val] == "%":
            if line[val + 1] == "b":
                format_list.append("0")
            elif line[val + 1] == "c":
                format_list.append("0")
            elif line[val + 1] == "w":
                format_list.append("1")
            elif line[val + 1] == "d":
                format_list.append("3")
            elif line[val + 1] == "s":
                format_list.append("2")
            elif (line[val + 1] == "1" and line[val + 2] == "x") or (
                line[val + 1] == "0" and line[val + 2] == "1" and line[val + 3] == "x"
            ):
                format_list.append("0")
            elif (line[val + 1] == "2" and line[val + 2] == "x") or (
                line[val + 1] == "0" and line[val + 2] == "2" and line[val + 3] == "x"
            ):
                format_list.append("1")
            elif (line[val + 1] == "4" and line[val + 2] == "x") or (
                line[val + 1] == "0" and line[val + 2] == "4" and line[val + 3] == "x"
            ):
                format_list.append("3")
            else:
                raise RuntimeError(
                    "Error: Wrong Format in - " + c_line + " in file -  " + filename
                )
    return format_list


cfiles = [
    os.path.join(root, name)
    for root, dirs, files in os.walk(search_path)
    for name in files
    if name.endswith(".c")
]

for doc in cfiles:
    with open(doc, "r", encoding="utf-8", errors="replace") as f:
        line = f.readline()
        while line != "":
            line = line.lstrip(" {").rstrip(" \n}")
            corrected_line = ""
            if line.startswith("SL_PRINTF"):
                corrected_line += line
                while not line.endswith(";"):
                    line = f.readline().lstrip(" {").rstrip(" \n}")
                    corrected_line += line
                line = corrected_line
                new_line = line.partition("SL_PRINTF")[2].lstrip(" (").rstrip(" \n};")
                new_line = new_line[: len(new_line) - 1 :]
                code_line = new_line.split(",")
                for element in range(len(code_line)):
                    code_line[element] = code_line[element].strip(' "')
                numarg = get_numargs(new_line)
                error_check(
                    new_line, code_line, preferred_database, numarg, f.name, line
                )
                error_check(new_line, code_line, database, numarg, f.name, line)
                preferred_ID = False
                for element in code_line:
                    if element.lower().startswith("preferredid"):
                        if element.split(":")[1].strip().lower() == "true":
                            preferred_ID = True
                if preferred_ID:
                    add_pref_db(new_line, code_line, preferred_database, f.name, line)
                else:
                    add_db(new_line, code_line, database, f.name, line)
            line = f.readline()

# creating a debug_auto_gen.h file and creating enum of all debug id's
timestr = time.strftime("%Y/%m/%d-%H:%M:%S")
with open(os.path.join(autogen_path, "debug_auto_gen.h"), "w+") as fp:
    fp.writelines(
        [
            "//! This is an Auto Generated Header file",
            "\n//! do not modifiy this file directly",
            "\n#ifndef DEBUGS_AUTOGEN_H",
            "\n#define DEBUGS_AUTOGEN_H",
        ]
    )
    fp.write("\n#define SL_LOG_VERSION  \"" + timestr + "\"")
    fp.write("\n\nenum {\n")
    string = ""
    combined_database = merge_db(preferred_database, database)
    for key in combined_database:
        string += key + ",\n"
    string = string.rstrip(",\n")
    fp.write(string)
    fp.writelines(["\n};", "\n"])
    fp.write("\n#define SL_PRINTF(DEBUG_ID, component, level,...) SL_PRINTF_##DEBUG_ID(DEBUG_ID, component, level, ##__VA_ARGS__)\n")
    debug_bits = "0xfff"
    arg1_start_bit = "12"
    arg2_start_bit = "14"
    arg3_start_bit = "16"
    component_start_bit = "18"
    level_start_bit = "21"
    enum_key = 0
    for key in combined_database:
        if combined_database[key]["argnum"] == 0:
            fp.write(
                "\n#define "
                + key
                + "_SIGNATURE(component, level)  (("
                + key
                + " & "
                + debug_bits
                + ") | (component << "
                + component_start_bit
                + ") | (level << "
                + level_start_bit
                + "))"
            )
        elif combined_database[key]["argnum"] == 1:
            fp.write(
                "\n#define "
                + key
                + "_SIGNATURE(component, level)  (("
                + key
                + " & "
                + debug_bits
                + ") | ("
                + combined_database[key]["arg1type"]
                + " << "
                + arg1_start_bit
                + ") | (component << "
                + component_start_bit
                + ") | (level << "
                + level_start_bit
                + "))"
            )
        elif combined_database[key]["argnum"] == 2:
            fp.write(
                "\n#define "
                + key
                + "_SIGNATURE(component, level)  (("
                + key
                + " & "
                + debug_bits
                + ") | ("
                + combined_database[key]["arg1type"]
                + " << "
                + arg1_start_bit
                + ") | ("
                + combined_database[key]["arg2type"]
                + " << "
                + arg2_start_bit
                + ") | (component << "
                + component_start_bit
                + ") | (level << "
                + level_start_bit
                + "))"
            )
        elif combined_database[key]["argnum"] == 3:
            fp.write(
                "\n#define "
                + key
                + "_SIGNATURE(component, level)  (("
                + key
                + " & "
                + debug_bits
                + ") | ("
                + combined_database[key]["arg1type"]
                + " << "
                + arg1_start_bit
                + ") | ("
                + combined_database[key]["arg2type"]
                + " << "
                + arg2_start_bit
                + ") | ("
                + combined_database[key]["arg3type"]
                + " << "
                + arg3_start_bit
                + ") | (component << "
                + component_start_bit
                + ") | (level << "
                + level_start_bit
                + "))"
            )
        temp_dict = {"debug_id": key}
        final_database[enum_key] = merge_db(temp_dict, combined_database[key])
        enum_key += 1
    fp.write("\n")
    for key in combined_database:
        fp.write(
            "\n#define "
            + get_new_slprintf(key, combined_database)
            + "    "
            + get_sl_log(key, combined_database)
        )
    fp.write("\n#endif")

# JSON Dump
final_database["SL_LOG_VERSION"] = timestr
with open(os.path.join(json_path, "manifest.json"), "w+") as fp:
    json.dump(final_database, fp)
