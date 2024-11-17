#!/bin/bash

# Function to display usage instructions
usage() {
    echo "Usage: $0 /path/to/program.c [-S | --lex | --parse | --codegen]"
    echo "      [ --debug ] "
    echo "  -S assembly generation without linking"
    echo "  --lex lexing, stop before parsing"
    echo "  --parse lexing and parsing, stop before code generation"
    echo "  --codegen lexing, parsing, and code generation, stop before code emission"
    echo "  --lex | parse | codegen don't emit output files, exit code 0 iff no error" 
    echo "  --debug write to stdout the intermediate results of all stages" 
    echo "  (may or may not silently ignore other arguments)"
}

if [ $# -lt 1 ]; then
    echo "Missing filename"; usage; exit 1; 
fi

# Assign the filename to a variable
filename=$1
shift 

opt_short="S"
opt_long="lex,parse,codegen,debug"

OPTS=$(getopt -o "$opt_short" -l "$opt_long" -- "$@")

if [ $? -ne 0 ] ; then
        echo "Unrecognized option."; 1>&2;
        usage; 
        exit 1;
fi

eval set -- "$OPTS"

S=0
lex=0
parse=0
codegen=0
debug=false

flag=""
 
while true
do 
    case "$1" in
        -S) S=1; shift;;
        --lex) lex=1; flag="--lex"; shift ;;
        --parse) parse=1; flag="--parse"; shift;;
        --codegen) codegen=1; flag="--codegen"; shift;;
        --debug) debug=true; shift ;;
        --) break ;;
    esac
done

if [ $((S + lex + parse + codegen)) -gt 1 ] ; then
    echo "Conflicting options: no more than one of [-S | --lex | --parse | --codegen]"; usage; 
    exit 1
fi

if [ ! -f "$filename" ]; then
    echo "Error: File '$filename' not found.";
    usage; 
    exit 1
fi

preprocess_file=$(dirname "$filename")/$(basename "${filename%%.*}").i

gcc -E -P $filename -o $preprocess_file

if [ $? -ne 0 ]; then 
    echo "GCC preprocessor failed."
    exit $?
fi

if [ ! -f "cynocompiler" ]; then
    echo "Error: Main compiler expected as filename 'cynocompiler' in same directory.";
    usage; 
    exit 1
fi

if $debug; then
    ./cynocompiler $preprocess_file --debug $flag
else
    ./cynocompiler $preprocess_file $flag 
fi

compile_code=$?
rm $preprocess_file

if [ $compile_code -ne 0 ]; then  
    if $debug; then 
        echo "Compilation failed." 
    fi
    exit $compile_code
fi

assembly_file=$(dirname "$filename")/$(basename "${filename%%.*}").s
output_file=$(dirname "$filename")/$(basename "${filename%%.*}")


if [ $((S + lex + parse + codegen)) -eq 0 ] ; then
    gcc $assembly_file -o $output_file
fi