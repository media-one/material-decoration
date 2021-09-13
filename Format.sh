#!/bin/sh

#set -e
clear

currentdir="$PWD"
config="$currentdir/Uncrustify.cfg"
prefix="$PWD"

. "$currentdir/JobPool.sh"

if [ $# -eq 0 ]; then
    set "$PWD"
fi

# Variables that will hold the name of the required binaries
iconv="iconv"
uncrustify="uncrustify"
clang_format="clang-format"

# Check if we found a working binaries
trap "exit 1" TERM
export top_pid=$$
find_executable_file() {
	if [ -z "${1}" ]; then
		echo "Error: Failed to find ${1}."
		kill -s TERM $top_pid
	fi
}

find_executable_file "${iconv}"
find_executable_file "${uncrustify}"
find_executable_file "${clang_format}"


# Find a dominating file, starting from a given directory and going up.
find_dominating_file() {
	if [ -r "$1"/"$2" ]; then
		#echo Using "$1"/"$2"
		return 0
	fi
	if [ "$1" = "/" ]; then
		return 1
	fi
	find_dominating_file "$(dirname "$1")" "$2"
	return $?
}

format_directory() {
	name=$(basename "$1")
	if [ "$name" = "lib_exports" ] || [ "$name" = "LibraryExports" ]; then
		filelist=$(find "$1" \
				-maxdepth 1 \
				-type f)
	else
		filelist=$(find "$1" \
				-maxdepth 1 \
				-type f \
				-name '*.c' \
			-o -name '*.cc' \
			-o -name '*.cpp' \
			-o -name '*.h' \
			-o -name '*.hh' \
			-o -name '*.hpp' \
			-o -name '*.mqh' \
			-o -name '*.mq4' \
			-o -name '*.mq5')
	fi

	for filename in ${filelist}; do
		if [ -w "${filename}" ]; then
			cmd="( "
			charset=$(file -i "${filename}" | grep -oP "charset=\K.*")
			if [ "$charset" != "us-ascii" ] && [ "$charset" != "utf-8" ] && [ "$charset" != "binary" ]; then
				cmd="${cmd} echo \"Converting ${filename#"$prefix"} from $charset to utf-8\";"
				cmd="${cmd} \"${iconv}\" -f \"$charset\" -t utf-8 \"${filename}\" -o \"${filename}.sbackup\";"
				cmd="${cmd} mv -f \"${filename}.sbackup\" \"${filename}\";"
			fi

			cmd="${cmd} echo \"Format ${filename#"$prefix"}\";"
			cmd="${cmd} \"${uncrustify}\" -q --no-backup --if-changed -l CPP -c \"${config}\" -f \"${filename}\" -o \"${filename}\";"
			cmd="${cmd} \"${clang_format}\" -i \"${filename}\";"
			cmd="${cmd} )"
			
			#echo $cmd
			#eval "${cmd}"
			job_pool_run eval "${cmd}"
		fi
	done
}

start=$(date +%s)
threads=$(nproc)
threads=$((threads))
echo Using ${threads} threads...
job_pool_init ${threads} 0

# Check all of the arguments first to make sure they're all directories
for param in "$@"; do
	if [ ! -d "${param}" ]; then
		echo "Parameter \"${param}\" is not a directory";
	else
        prefix="${param}/"
		dirlist=$(find "${param}" -type d)
		for directory in ${dirlist}; do
			current="${directory#"$prefix"}"
			if ! find_dominating_file "${directory}" ".clang-format"; then
				echo "Failed to find dominating .clang-format starting at \"${current}\"."
				continue
			fi
			if find_dominating_file "${directory}" ".no-format"; then
				echo "Ignoring directory \"${current}\"."
				continue
			fi
			format_directory "${directory}";
		done
	fi
done

job_pool_wait
job_pool_shutdown

# Finally, clean ccache.
ccache -c -C -z

end=$(date +%s)
echo "~~~ Done in $(expr $end - $start) seconds ~~~";

# $Id$
