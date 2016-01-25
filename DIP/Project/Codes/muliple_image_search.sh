#! /bin/bash

# function to create all dirs til file can be made
function mkdirs {
    file="$1"
    dir="/"

    # convert to full path
    if [ "${file##/*}" ]; then
        file="${PWD}/${file}"
    fi

    # dir name of following dir
    next="${file#/}"

    # while not filename
    while [ "${next//[^\/]/}" ]; do
        # create dir if doesn't exist
        [ -d "${dir}" ] || mkdir "${dir}"
        dir="${dir}/${next%%/*}"
        next="${next#*/}"
    done

    # last directory to make
    [ -d "${dir}" ] || mkdir "${dir}"
}


# parse arguments
#if [ ${1} -eq "--help" ]
#then
#    echo -e "Usage: <prog> <search_string> <max_no._of_downloads>\nAll the spaThe max_no._of_downloads argument is optional."
#fi
echo "hello"
read -p "Search string: " query
query=`echo $query | sed 's/ /+/g'`
[ -z "$query" ] && exit 1  # insufficient arguments

read -p "Max no. of downloads (default is 20. Press enter for default): " limit
if [ -z "$limit" ]
then
    limit=20
fi

# set user agent, customize this by visiting http://whatsmyuseragent.com/
useragent='Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:31.0) Gecko/20100101 Firefox/31.0'

# construct google link
link="www.google.cz/search?q=${query}\&tbm=isch"

# fetch link for download

if ! [ -d "${PWD}/${query}" ];
then
    mkdir "${PWD}/${query}"
fi
dir="${PWD}/${query}"

count=1
imagelinks=$(wget -e robots=off --user-agent "$useragent" -qO - "$link" | sed 's/</\n</g' | grep '<a href.*\(png\|jpg\|jpeg\)' | sed 's/.*imgurl=\([^&]*\)\&.*/\1/')
for imagelink in `echo $imagelinks`
do
    if [ $count -gt $limit ]
    then
        break
    fi
    imagelink="${imagelink%\%*}"
    # get file extention (.png, .jpg, .jpeg)
    ext=$(echo $imagelink | sed "s/.*\(\.[^\.]*\)$/\1/")
    if [ "${ext}" != ".jpg" ] && [ "${ext}" != ".jpeg" ]
    then
        echo $ext
        continue
    fi
    filename="${dir}/${count}${ext}"
    # get actual picture and store in $count.$ext
    wget --max-redirect 0 -qO "${filename}" "${imagelink}"
    ((count=count+1))
done

# successful execution, exit code 0
exit 0